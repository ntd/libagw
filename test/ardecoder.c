/* libagw - Additional GTK Widgets
 * Copyright (C) 2020  Nicola Fontana <ntd@entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <gtk/gtk.h>
#include <libserialport.h>
#include <stdio.h>
#include "../src/agw-gauge.h"

#define THREAD_QUIT()   g_atomic_int_set(&quit, TRUE)


static gchar *device = NULL;
static gint ppr = 2000;
static gboolean inverted = FALSE;
static GtkWidget *gauge = NULL;
static volatile gboolean quit = FALSE;
static GThread *encoder1_thread = NULL;


static void
serial_error(struct sp_port *port)
{
    if (port == NULL) {
        g_warning("\"%s\" is not a valid serial device", device);
    } else {
        char *message = sp_last_error_message();
        g_warning("Communication error(%d) [%s] \"%s\"",
                  sp_last_error_code(), sp_get_port_name(port), message);
        sp_free_error_message(message);
    }
}

static void
serial_abort(struct sp_port *port)
{
    serial_error(port);
    THREAD_QUIT();
}

static void
serial_close(struct sp_port *port)
{
    if (port != NULL) {
        sp_close(port);
        sp_free_port(port);
    }
}

static struct sp_port *
serial_open(void)
{
    struct sp_port *port = NULL;

    /* Configure the communication */
    if (sp_get_port_by_name(device, &port) != SP_OK ||
        sp_open(port, SP_MODE_READ_WRITE) != SP_OK ||
        sp_set_baudrate(port, 115200) != SP_OK ||
        sp_set_bits(port, 8) != SP_OK ||
        sp_set_stopbits(port, 1) != SP_OK ||
        sp_set_parity(port, SP_PARITY_NONE) != SP_OK ||
        sp_set_flowcontrol(port, SP_FLOWCONTROL_RTSCTS) != SP_OK) {
        /* Error while opening the serial communication */
        serial_error(port);
        serial_close(port);
        return NULL;
    }

    return port;
}

static gchar *
serial_receive(struct sp_port *port, guint timeout)
{
    gchar byte;
    int status;
    GString *string = g_string_sized_new(10);

    for (;;) {
        status = sp_blocking_read(port, &byte, 1, timeout);
        if (status < 0) {
            /* Error while receiving: abort the thread */
            serial_abort(port);
            g_string_free(string, TRUE);
            return NULL;
        } else if (status == 0) {
            /* Timeout: just return NULL */
            g_string_free(string, TRUE);
            return NULL;
        }
        /* First timeout (from request to start of response) is custom.
         * Subsequent bytes have 0,1 second fixed limit instead */
        timeout = 100;
        if (byte == '\n') {
            break;
        } else if (byte != '\r') {
            string = g_string_append_c(string, byte);
        }
    }
    return g_string_free(string, FALSE);
}

static gboolean
serial_send(struct sp_port *port, const gchar *text)
{
    int count = strlen(text);
    size_t status = sp_blocking_write(port, text, count, 500);
    if (status < 0) {
        /* Error while sending: abort the thread */
        serial_abort(port);
        return FALSE;
    } else if (status != count) {
        /* Timeout in sending: abort the thread */
        g_warning("Timeout sending \"%s\" to \"%s\" device", text, device);
        THREAD_QUIT();
        return FALSE;
    }
    return TRUE;
}

static gboolean
update_gauge(gpointer user_data)
{
    /* During shutdown `gauge` can be invalid */
    if (AGW_IS_GAUGE(gauge)) {
        agw_gauge_set_value(AGW_GAUGE(gauge), GPOINTER_TO_INT(user_data));
    }
    return G_SOURCE_REMOVE;
}

static gpointer
encoder_loop(gpointer user_data)
{
    gchar *line;
    gint n, value, homing;
    struct sp_port *port;
    gboolean boot = TRUE;

    port = serial_open();
    if (port == NULL) {
        return NULL;
    }

    while (g_atomic_int_get(&quit) == FALSE) {
        line = serial_receive(port, 1000);
        if (line == NULL) {
            /* Timeout: do nothing */
        } else if (line[0] == '#') {
            /* Comment */
            g_message("%s", line + 1);
            if (boot) {
                /* ardecoder up and running: enable push-mode (0,1 s) */
                serial_send(port, "S100\n");
                serial_send(port, "1\n");
                boot = FALSE;
            }
        } else if (line[0] == '?') {
            /* Error */
            g_warning("%s", line + 1);
        } else {
            /* Data line */
            sscanf(line, "%d %d %d", &n, &value, &homing);
            g_debug("Encoder %d: %d%s", n, value, homing ? "" : " not homed");
            g_main_context_invoke(NULL, update_gauge, GINT_TO_POINTER(value));
        }
        g_free(line);
    }

    serial_close(port);
    return NULL;
}

static void
create_gauge(void)
{
    GError *error;
    gchar *theme;

    gauge = agw_gauge_new();
    /* A quadrature encoder has four AB states for every phase pulse
     * (00, 01, 10 and 11), so the total resolution is `4ppr`.
     * Splitting by 2ppr per side here. */
    gtk_range_set_range(GTK_RANGE(gauge), -2 * ppr, 2 * ppr);
    gtk_range_set_inverted(GTK_RANGE(gauge), inverted);

    error = NULL;
    theme = g_build_filename(SRCDIR, "assets", NULL);
    agw_gauge_set_theme(AGW_GAUGE(gauge), theme, &error);
    g_free(theme);

    /* Check if the gauge theme has been loaded properly */
    if (error != NULL) {
        g_log(g_quark_to_string(error->domain), G_LOG_LEVEL_CRITICAL,
              "[%d]: %s", error->code, error->message);
        g_error_free(error);
        g_object_unref(G_OBJECT(gauge));
    }
}

static void
on_activate(GtkApplication *app)
{
    GtkWidget *window;

    create_gauge();
    if (gauge == NULL) {
        /* Error while creating the gauge widget: bail out */
        return;
    }

    /* Create the user interface */
    window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 480);
    gtk_container_add(GTK_CONTAINER(window), gauge);
    gtk_widget_show_all(window);

    /* Start the encoder thread, if requested */
    if (device != NULL) {
        encoder1_thread = g_thread_new("encoder1", encoder_loop, NULL);
    }
}

static void
on_shutdown(GtkApplication *app)
{
    if (encoder1_thread != NULL) {
        THREAD_QUIT();
        g_thread_join(encoder1_thread);
        encoder1_thread = NULL;
    }
}

int
main(int argc, char **argv)
{
    GOptionEntry options[] = {{
        "device",                   /* long_name */
        'd',                        /* short_name */
        G_OPTION_FLAG_IN_MAIN,      /* flags */
        G_OPTION_ARG_FILENAME,      /* arg */
        &device,                    /* arg_data */
        "ardecoder serial device",  /* description */
        "DEVICE"                    /* arg_description */
    }, {
        "ppr",                      /* long_name */
        'p',                        /* short_name */
        G_OPTION_FLAG_IN_MAIN,      /* flags */
        G_OPTION_ARG_INT,           /* arg */
        &ppr,                       /* arg_data */
        "Pulses per revolution",    /* description */
        "N"                         /* arg_description */
    }, {
        "inverted",                 /* long_name */
        'i',                        /* short_name */
        G_OPTION_FLAG_IN_MAIN,      /* flags */
        G_OPTION_ARG_NONE,          /* arg */
        &inverted,                  /* arg_data */
        "Invert gauge movement",    /* description */
        NULL                        /* arg_description */
    }, {
        NULL
    }};
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.entidi.ardecoder", G_APPLICATION_FLAGS_NONE);
    g_application_add_main_option_entries(G_APPLICATION(app), options);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    g_signal_connect(app, "shutdown", G_CALLBACK(on_shutdown), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
