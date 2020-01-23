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


/**
 * agw_init:
 *
 * Initialize libagw. Theoretically this function is bogus but using 
 * AgwGauge in a GtkBuilder file leads up to a segmentation fault:
 * ```
 * Thread 1 ... received signal SIGTRAP, Trace/breakpoint trap.
 * 0x00007ffff74cc166 in ?? () from /usr/lib/libglib-2.0.so.0
 * (gdb) bt
 * #0  0x00007ffff74cc166 in  () at /usr/lib/libglib-2.0.so.0
 * #1  0x00007ffff74c90bb in g_log_structured_standard () at /usr/lib/libglib-2.0.so.0
 * #2  0x00007ffff7c2f015 in gtk_builder_new_from_file () at /usr/lib/libgtk-3.so.0
 * ...
 * ```
 *
 * Calling agw_gauge_get_type() before loading the builder file solves
 * the issue.
 **/
void
agw_init(void)
{
    agw_gauge_get_type();
}
