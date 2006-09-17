/* Copyright (C) 2006  Movial Oy
 * authors:     re@welho.com
 *              arno.karatmaa@gmail.com
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/sice_interface.h"

#ifndef _SICE_SIGNAL_HANDLER_H_
#define _SICE_SIGNAL_HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MAXSESSIONS 3
#define MAXTUNNELS 10

typedef struct _socket_tunnel                   socket_tunnel;
struct _socket_tunnel {
  addr4                 local_lib_addr;
  struct sockaddr_in    local_lib_sockaddr;
  int                   local_lib_socket_fd;
  
  addr4                 network_lib_addr;
  struct sockaddr_in    network_lib_sockaddr;
  int                   network_lib_socket_fd;
};

typedef struct _session_data                    session_data;
struct _session_data {
  int             port_count;
  addr4           client_ports[MAXTUNNELS];
  
  socket_tunnel   socket_tunnels[MAXTUNNELS];
  GQueue*         event_fifo;
  GQueue*         timed_task_list;
};


int
create_socket (                 int                             port,
                                struct sockaddr_in*             native_addr_pointer,
                                int*                            socket_fd);

int
signal_handler_initialize (     int                             session_id,
                                int                             port_count,
                                int*                            client_ports,
                                int*                            library_ports,
                                sice_callback                   callback );

void run_signal_handler (       int                             client_port_count,
                                int*                            client_ports, 
                                GQueue*                         event_fifo, 
                                GQueue*                         timed_task_list,
                                session_data                    session );

#ifdef __cplusplus
}
#endif

#endif /* _SICE_SIGNAL_HANDLER_H_ */
