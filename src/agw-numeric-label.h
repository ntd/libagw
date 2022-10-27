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

#ifndef __AGW_NUMERIC_LABEL_H__
#define __AGW_NUMERIC_LABEL_H__

#include <gtk/gtk.h>


G_BEGIN_DECLS

#define AGW_TYPE_NUMERIC_LABEL agw_numeric_label_get_type()

G_DECLARE_FINAL_TYPE(AgwNumericLabel, agw_numeric_label, AGW, NUMERIC_LABEL, GtkLabel)


GtkWidget *     agw_numeric_label_new           (void);
void            agw_numeric_label_set_format    (AgwNumericLabel *  label,
                                                 const gchar *      format);
const gchar *   agw_numeric_label_get_format    (AgwNumericLabel *  label);
void            agw_numeric_label_set_value     (AgwNumericLabel *  label,
                                                 gdouble            value);
gdouble         agw_numeric_label_get_value     (AgwNumericLabel *  label);
void            agw_numeric_label_update_text   (AgwNumericLabel *  label);

G_END_DECLS


#endif /* __AGW_NUMERIC_LABEL_H__ */
