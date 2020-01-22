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

#ifndef __AGW_GAUGE_H__
#define __AGW_GAUGE_H__

#include <gtk/gtk.h>


G_BEGIN_DECLS

#define AGW_TYPE_GAUGE agw_gauge_get_type()

G_DECLARE_FINAL_TYPE(AgwGauge, agw_gauge, AGW, GAUGE, GtkRange)


GtkWidget *     agw_gauge_new               (void);
gboolean        agw_gauge_set_theme         (AgwGauge *     gauge,
                                             const gchar *  theme_dir,
                                             GError **      error);
void            agw_gauge_set_value         (AgwGauge *     gauge,
                                             gdouble        value);

G_END_DECLS

#endif /* __AGW_GAUGE_H__ */
