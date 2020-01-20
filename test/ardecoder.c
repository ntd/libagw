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
#include "../src/agw-gauge.h"


static GtkWidget *
create_gauge(void)
{
    GtkWidget *gauge;
    GError *error;
    gchar *theme;

    gauge = agw_gauge_new();
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
        return NULL;
    }

    return gauge;
}

static void
on_activate(GtkApplication *app)
{
    GtkWidget *window;
    GtkWidget *gauge;

    gauge = create_gauge();
    if (gauge == NULL) {
        /* Error while creating the gauge widget: bail out */
        return;
    }

    /* Create the user interface */
    window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 480);
    gtk_container_add(GTK_CONTAINER(window), gauge);
    gtk_widget_show_all(window);
}

int
main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.entidi.ardecoder", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
