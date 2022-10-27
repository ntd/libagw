/* libagw - Additional GTK Widgets
 * Copyright (C) 2022  Nicola Fontana <ntd@entidi.it>
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

/**
 * SECTION:agw-numeric-label
 * @short_description: A `GtkLabel` with a `value` property
 *
 * This comes in handy when you want to display a value. Having it as a
 * property is also useful to bind it to a numerical GSettings key.
 *
 * You can customize the way the value is formatted by using
 * agw_numeric_label_set_format(): that string will be passed directly
 * to sprintf(), so be sure to include one (and only one)
 * `%f`-compatible argument. By default the format is set to `"%g"`.
 **/

/**
 * AgwNumericLabel:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 **/

#include "agw-numeric-label.h"


typedef struct {
    gchar * format;
    gdouble value;
} AgwNumericLabelPrivate;

struct _AgwNumericLabel {
    GtkLabel parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE(AgwNumericLabel, agw_numeric_label, GTK_TYPE_LABEL)

enum {
    PROP_0,
    PROP_FORMAT,
    PROP_VALUE,
    NUM_PROPERTIES,
};

static GParamSpec *props[NUM_PROPERTIES] = { 0 };


static void
finalize(GObject *object)
{
    AgwNumericLabel *label = AGW_NUMERIC_LABEL(object);
    AgwNumericLabelPrivate *priv = agw_numeric_label_get_instance_private(label);
    if (priv->format != NULL) {
        g_free(priv->format);
        priv->format = NULL;
    }
}

static void
get_property(GObject *object, guint prop_id,
             GValue *value, GParamSpec *pspec)
{
    AgwNumericLabel *label = AGW_NUMERIC_LABEL(object);
    AgwNumericLabelPrivate *priv = agw_numeric_label_get_instance_private(label);

    switch (prop_id) {
    case PROP_FORMAT:
        g_value_set_string(value, priv->format);
        break;
    case PROP_VALUE:
        g_value_set_double(value, priv->value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
set_property(GObject *object, guint prop_id,
             const GValue *value, GParamSpec *pspec)
{
    AgwNumericLabel *label = AGW_NUMERIC_LABEL(object);

    switch (prop_id) {
    case PROP_FORMAT:
        agw_numeric_label_set_format(label, g_value_get_string(value));
        break;
    case PROP_VALUE:
        agw_numeric_label_set_value(label, g_value_get_double(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


static void
agw_numeric_label_class_init(AgwNumericLabelClass *class)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS(class);
    gobject_class->finalize = finalize;
    gobject_class->get_property = get_property;
    gobject_class->set_property = set_property;

    props[PROP_FORMAT] = g_param_spec_string("format",
                                             "printf-style Format String",
                                             "The format to use for rendering the value (in printf-style)",
                                             "%g",
                                             G_PARAM_READWRITE);
    props[PROP_VALUE] = g_param_spec_double("value",
                                            "Value",
                                            "Value to display",
                                            -G_MAXDOUBLE, G_MAXDOUBLE, 0,
                                            G_PARAM_READWRITE);

    g_object_class_install_properties(gobject_class, NUM_PROPERTIES, props);
}

static void
agw_numeric_label_init(AgwNumericLabel *label)
{
    gtk_label_set_use_markup(GTK_LABEL(label), FALSE);
}


/**
 * agw_numeric_label_new:
 *
 * Creates a new #AgwNumericLabel widget.
 *
 * Returns: the newly created widget
 **/
GtkWidget *
agw_numeric_label_new(void)
{
    GtkWidget *widget = (GtkWidget *) g_object_new(AGW_TYPE_NUMERIC_LABEL, NULL);
    return widget;
}

/**
 * agw_numeric_label_set_format:
 * @label: an #AgwNumericLabel
 * @format: the new format to adopt
 *
 * Sets a new format string to use for @label. This string is passed
 * directly to sprintf(), so be sure to include one (and only one)
 * `%f`-compatible argument. By default the format is set to `"%g"`.
 *
 * Changing the format triggers the update of the text with an
 * agw_numeric_label_update_text() call.
 **/
void
agw_numeric_label_set_format(AgwNumericLabel *label, const gchar *format)
{
    AgwNumericLabelPrivate *priv;

    g_return_if_fail(AGW_IS_NUMERIC_LABEL(label));

    /* g_strcmp0 and g_strdup already handle NULL gracefully */
    priv = agw_numeric_label_get_instance_private(label);
    if (g_strcmp0(format, priv->format) != 0) {
        g_free(priv->format);
        priv->format = g_strdup(format);
        g_object_notify_by_pspec(G_OBJECT(label), props[PROP_FORMAT]);
        agw_numeric_label_update_text(label);
    }
}

/**
 * agw_numeric_label_get_format:
 * @label: an #AgwNumericLabel
 *
 * Gets the current label format.
 *
 * @return: the current label format
 **/
const gchar *
agw_numeric_label_get_format(AgwNumericLabel *label)
{
    AgwNumericLabelPrivate *priv;

    g_return_val_if_fail(AGW_IS_NUMERIC_LABEL(label), NULL);

    priv = agw_numeric_label_get_instance_private(label);
    return priv->format;
}

/**
 * agw_numeric_label_set_value:
 * @label: an #AgwNumericLabel
 * @value: new value
 *
 * Sets a new value for the label. This in turn triggers the update of
 * the text with an agw_numeric_label_update_text() call.
 **/
void
agw_numeric_label_set_value(AgwNumericLabel *label, gdouble value)
{
    AgwNumericLabelPrivate *priv;

    g_return_if_fail(AGW_IS_NUMERIC_LABEL(label));

    priv = agw_numeric_label_get_instance_private(label);
    if (value != priv->value) {
        priv->value = value;
        g_object_notify_by_pspec(G_OBJECT(label), props[PROP_VALUE]);
        agw_numeric_label_update_text(label);
    }
}

/**
 * agw_numeric_label_get_value:
 * @label: an #AgwNumericLabel
 *
 * Gets the current label value.
 *
 * @return: the current label value
 **/
gdouble
agw_numeric_label_get_value(AgwNumericLabel *label)
{
    AgwNumericLabelPrivate *priv;

    g_return_val_if_fail(AGW_IS_NUMERIC_LABEL(label), 0);

    priv = agw_numeric_label_get_instance_private(label);
    return priv->value;
}

/**
 * agw_numeric_label_update_text:
 * @label: an #AgwNumericLabel
 *
 * Updates the text according to the numeric label value and format
 * properties. This method is seldomly useful outside of
 * #AgwNumericLabel subclasses.
 **/
void
agw_numeric_label_update_text(AgwNumericLabel *label)
{
    AgwNumericLabelPrivate *priv;
    gchar *text;

    g_return_if_fail(AGW_IS_NUMERIC_LABEL(label));

    priv = agw_numeric_label_get_instance_private(label);
    text = g_strdup_printf(priv->format, priv->value);
    gtk_label_set_text(GTK_LABEL(label), text);
    g_free(text);
}
