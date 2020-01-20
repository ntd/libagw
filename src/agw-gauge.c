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

#include "agw-gauge.h"
#include <math.h>
#include <librsvg/rsvg.h>


typedef enum {
    AGW_GAUGE_ELEMENT_DROP_SHADOW,
    AGW_GAUGE_ELEMENT_FACE,
    AGW_GAUGE_ELEMENT_MARKS,
    AGW_GAUGE_ELEMENT_HOUR_HAND_SHADOW,
    AGW_GAUGE_ELEMENT_MINUTE_HAND_SHADOW,
    AGW_GAUGE_ELEMENT_SECOND_HAND_SHADOW,
    AGW_GAUGE_ELEMENT_HOUR_HAND,
    AGW_GAUGE_ELEMENT_MINUTE_HAND,
    AGW_GAUGE_ELEMENT_SECOND_HAND,
    AGW_GAUGE_ELEMENT_FACE_SHADOW,
    AGW_GAUGE_ELEMENT_GLASS,
    AGW_GAUGE_ELEMENT_FRAME,
    AGW_GAUGE_ELEMENT_LAST,
} AgwGaugeElement;

typedef struct {
    RsvgHandle *svg[AGW_GAUGE_ELEMENT_LAST];
    gint        width;
    gint        height;
} AgwGaugePrivate;

struct _AgwGauge {
    GtkRange parent_instance;
};


static const gchar *theme_file[AGW_GAUGE_ELEMENT_LAST] = {
    "clock-drop-shadow.svg",
    "clock-face.svg",
    "clock-marks.svg",
    "clock-hour-hand-shadow.svg",
    "clock-minute-hand-shadow.svg",
    "clock-second-hand-shadow.svg",
    "clock-hour-hand.svg",
    "clock-minute-hand.svg",
    "clock-second-hand.svg",
    "clock-face-shadow.svg",
    "clock-glass.svg",
    "clock-frame.svg",
};


G_DEFINE_TYPE_WITH_PRIVATE(AgwGauge, agw_gauge, GTK_TYPE_RANGE)


static void
get_preferred_width_or_height(GtkWidget *widget, gint *minimum, gint *natural)
{
    *minimum = 64;
    *natural = 200;
}

static gboolean
draw(GtkWidget *widget, cairo_t *cr)
{
    AgwGauge *gauge = AGW_GAUGE(widget);
    AgwGaugePrivate *priv = agw_gauge_get_instance_private(gauge);
    GtkRange *range = GTK_RANGE(widget);
    GtkAdjustment *adjustment = gtk_range_get_adjustment(range);
    GtkAllocation room;
    gdouble size, angle;

    gtk_widget_get_allocation(widget, &room);
    size = MIN(room.width, room.height);

    cairo_translate(cr, (room.width - size) / 2, (room.height - size) / 2);
    cairo_scale(cr, size / priv->width, size / priv->height);

    /* Clear the surface */
    cairo_set_source_rgba(cr, 1, 1, 1, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_paint(cr);

    /* Draw the background */
    rsvg_handle_render_cairo(priv->svg[AGW_GAUGE_ELEMENT_DROP_SHADOW], cr);
    rsvg_handle_render_cairo(priv->svg[AGW_GAUGE_ELEMENT_FACE], cr);
    rsvg_handle_render_cairo(priv->svg[AGW_GAUGE_ELEMENT_MARKS], cr);

    /* Draw the hand */
    angle = gtk_adjustment_get_value(adjustment) *
            G_PI / gtk_adjustment_get_upper(adjustment);
    if (gtk_range_get_inverted(range)) {
        angle = -angle;
    }
    angle -= G_PI_2;

    cairo_save(cr);
    cairo_translate(cr, priv->width / 2, priv->height / 2);
    cairo_save(cr);
    cairo_translate(cr, -0.75, 0.75);
    cairo_rotate(cr, angle);
    rsvg_handle_render_cairo(priv->svg[AGW_GAUGE_ELEMENT_MINUTE_HAND_SHADOW], cr);
    cairo_restore(cr);
    cairo_rotate(cr, angle);
    rsvg_handle_render_cairo(priv->svg[AGW_GAUGE_ELEMENT_MINUTE_HAND], cr);
    cairo_restore(cr);

    /* Draw the foreground */
    rsvg_handle_render_cairo(priv->svg[AGW_GAUGE_ELEMENT_FACE_SHADOW], cr);
    rsvg_handle_render_cairo(priv->svg[AGW_GAUGE_ELEMENT_GLASS], cr);
    rsvg_handle_render_cairo(priv->svg[AGW_GAUGE_ELEMENT_FRAME], cr);

    return FALSE;
}

static void
free_svg(AgwGaugePrivate *priv)
{
    RsvgHandle **svg;
    gint i;

    for (i = 0; i < AGW_GAUGE_ELEMENT_LAST; ++i) {
        svg = priv->svg + i;
        if (*svg != NULL) {
            g_object_unref(G_OBJECT(*svg));
            *svg = NULL;
        }
    }
}

static gboolean
set_theme(AgwGaugePrivate *priv, const gchar *theme_dir, GError **error)
{
    gchar *file;
    RsvgHandle *svg;
    RsvgDimensionData dimension;
    gint i;

    priv->width  = 0;
    priv->height = 0;
    free_svg(priv);

    for (i = 0; i < AGW_GAUGE_ELEMENT_LAST; ++i) {
        file = g_build_filename(theme_dir, theme_file[i], NULL);
        svg  = rsvg_handle_new_from_file(file, error);
        g_free(file);

        /* On errors, return false without further processing */
        if (svg == NULL) {
            g_assert(error == NULL || *error != NULL);
            return FALSE;
        }

        /* Get the extents of the biggest element */
        rsvg_handle_get_dimensions(svg, &dimension);
        priv->width  = MAX(dimension.width, priv->width);
        priv->height = MAX(dimension.height, priv->height);

        priv->svg[i] = svg;
    }

    return TRUE;
}

static void
finalize(GObject *object)
{
    AgwGauge *gauge = AGW_GAUGE(object);
    AgwGaugePrivate *priv = agw_gauge_get_instance_private(gauge);
    free_svg(priv);
}

static void
agw_gauge_class_init(AgwGaugeClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);

    object_class->finalize = finalize;

    widget_class->get_preferred_width = get_preferred_width_or_height;
    widget_class->get_preferred_height = get_preferred_width_or_height;
    widget_class->draw = draw;
}

static void
agw_gauge_init(AgwGauge *gauge)
{
    AgwGaugePrivate *priv = agw_gauge_get_instance_private(gauge);
    gchar *theme;
    gint i;

    gtk_widget_set_has_window(GTK_WIDGET(gauge), FALSE);

    /* Set the default theme */
    for (i = 0; i < AGW_GAUGE_ELEMENT_LAST; ++i) {
        priv->svg[i] = NULL;
    }
    theme = g_build_filename(PKGDATADIR, "assets", NULL);
    set_theme(priv, theme, NULL);
    g_free(theme);
}


/**
 * agw_gauge_new:
 *
 * Creates a new #AgwGauge widget.
 *
 * Returns: the newly created widget
 **/
GtkWidget *
agw_gauge_new(void)
{
    GtkWidget *widget = (GtkWidget *) g_object_new(AGW_TYPE_GAUGE, NULL);
    GtkRange *range   = GTK_RANGE(widget);
    gtk_range_set_adjustment(range, NULL);
    gtk_range_set_range(range, -100, 100);
    return widget;
}

/**
 * agw_gauge_set_theme:
 * @gauge: an #AgwGauge
 * @theme_dir: path to the new theme
 * @error: (allow-none): return location for a #GError, or %NULL
 *
 * Sets the new theme on @gauge. A theme is a collection of SVG files
 * with specific names and @theme_dir is the path to the folder
 * containing those files. The conventions used are intentionally
 * compatible with the [cairo-clock](https://launchpad.net/cairo-clock)
 * project, so any cairo-clock theme can be used.
 *
 * @return: TRUE if the theme has been succesfully changed.
 **/
gboolean
agw_gauge_set_theme(AgwGauge *gauge, const gchar *theme_dir, GError **error)
{
    AgwGaugePrivate *priv;

    g_return_val_if_fail(AGW_IS_GAUGE(gauge), FALSE);
    g_return_val_if_fail(theme_dir != NULL, FALSE);
    g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

    priv = agw_gauge_get_instance_private(gauge);
    return set_theme(priv, theme_dir, error);
}

/**
 * agw_gauge_set_value:
 * @gauge: an #AgwGauge
 * @value: new value of the gauge
 *
 * Sets the current value of the gauge; if the value is outside the
 * minimum or maximum range values, it will be wrapped around to fit
 * inside them. The gauge emits the #AgwGauge::value-changed signal if
 * the value changes.
 **/
void
agw_gauge_set_value(AgwGauge *gauge, gdouble value)
{
    GtkAdjustment *adjustment;
    gdouble lower, upper;

    g_return_if_fail(AGW_IS_GAUGE(gauge));

    adjustment = gtk_range_get_adjustment(GTK_RANGE(gauge));
    lower = gtk_adjustment_get_lower(adjustment);
    upper = gtk_adjustment_get_upper(adjustment);

    while (value < lower) {
        value += upper - lower;
    }
    while (value > upper) {
        value -= upper - lower;
    }

    gtk_adjustment_set_value(adjustment, value);
}
