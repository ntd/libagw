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
#include "agw-numeric-label.h"


/**
 * agw_init:
 *
 * Initialize libagw.
 *
 * Using any widget from GtkBuilder requires it to be already present
 * in the type system, so this function ensures every AGW widget is
 * registered.
 **/
void
agw_init(void)
{
    g_type_ensure(AGW_TYPE_GAUGE);
    g_type_ensure(AGW_TYPE_NUMERIC_LABEL);
}
