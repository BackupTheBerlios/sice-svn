/* Copyright (C) 2006  Movial Oy
 * authors:     rami.erlin@movial.fi
 *              arno.karatmaa@movial.fi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,MA 02111-1307 USA 16
 */

#include <glib.h>

#ifndef _SICE_SIGNAL_HANDLER_H_
#define _SICE_SIGNAL_HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

void run_signal_handler(int client_port_count, int* client_ports, GQueue* event_fifo, GQueue* timed_task_list);

#ifdef __cplusplus
}
#endif

#endif /* _SICE_SIGNAL_HANDLER_H_ */
