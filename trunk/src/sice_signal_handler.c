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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../include/sice_types.h"
#include "sice_timer.h"
#include "sice_signal_handler.h"
#include "sice_main_state_machine.h"

#define PORT 9000               /* port we're listening on */

session_data sessions[MAXSESSIONS];

void
init_session(   session_data*           session) {
  (*session).event_fifo = NULL;
  (*session).timed_task_list = NULL;
}

int 
create_any_socket ( int*                    initial_trial_port,
                    int*                    port,
                    struct sockaddr_in*     native_addr_pointer,
                    int*                    socket_fd ) {
    int max_trial_port;
    int current_port;
    int retval;

    max_trial_port = *initial_trial_port + 20;
    current_port = *initial_trial_port;

    while ( current_port < max_trial_port ) {
        retval = create_socket ( current_port, native_addr_pointer, socket_fd );
        if (retval == 0) {
            *port = current_port;
            *initial_trial_port = current_port + 1;
            return 0;
        }
        current_port++;
        printf("Trying another socket!!!! %d failed\n", current_port);
    }
    return -1;
}

int
create_socket ( int                     port, 
                struct sockaddr_in*     native_addr_pointer,
                int*                    socket_fd ) {

    int listener;
    struct sockaddr_in native_addr;
    native_addr = *native_addr_pointer;

    /* struct sockaddr_in native_addr; */
    int yes = 1;

    printf("SOCKET (%d) \n", port);

    if ((listener = socket (PF_INET, SOCK_DGRAM, 0)) == -1) {
        printf ("ERROR socket\n");
        return 1;
    }

    /* lose the pesky "address already in use" error message */
    if (setsockopt (listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1) {
        printf ("ERROR setsocketopt\n");
        perror ("setsockopt");
        return 1;
    }

    native_addr.sin_family = AF_INET;
    native_addr.sin_addr.s_addr = INADDR_ANY;
    printf("INADDR_ANY: %d\n",  INADDR_ANY);
    native_addr.sin_port = htons (port);
    memset ((native_addr.sin_zero), '\0', 8);
    if (bind (listener, (struct sockaddr *) &native_addr, sizeof (native_addr)) == -1) {
        printf ("ERROR bind\n");
        perror ("bind");
        return 1;
    }

    *socket_fd = listener;
    return 0;
}

int
signal_handler_initialize (             int                     session_id,
                                        int                     port_count,
                                        int*                    client_ports,
                                        int*                    library_ports,
                                        sice_callback           callback ) {
  int j;
  addr4 address;
  char ip[4] = {127, 0, 0, 1}; /* address of localhost */
  int local_port;
  int initial_trial_port = 9000;
  session_data session;
  socket_tunnel tunnel;
  int retval;

  session = sessions[session_id];

  init_session(&session);

  sessions[session_id].port_count = port_count;
  for (j = 0; j < port_count; j++) {
      tunnel = session.socket_tunnels[j];

      /* save client ports for this session */
      address = sessions[session_id].client_ports[j];
      address.port = client_ports[j];
      address.addr = (guint32) ip;

      /* create local library socket */
      retval = create_any_socket( &initial_trial_port, &local_port, &tunnel.local_lib_sockaddr, 
              &tunnel.local_lib_socket_fd );
      if (retval == -1) 
          return -1;
      library_ports[j] = local_port; /* return value for the client */
      tunnel.local_lib_addr.port = local_port;
      tunnel.local_lib_addr.addr = (guint32) ip;

      /* create network library socket */
      retval = create_any_socket( &initial_trial_port, &local_port, &tunnel.local_lib_sockaddr,
              &tunnel.local_lib_socket_fd );
    if (retval == -1)
        return -1;
    tunnel.network_lib_addr.port = local_port;
    tunnel.network_lib_addr.addr = (guint32) ip;

  }

  printf("Initialising signal handler sockets!\n");
  return 0;
}


/* Getting delay for the next time task if any, if not, returning NULL */

struct timeval* 
set_timed_task_list_delay(      GQueue*         timed_task_list,
                                struct timeval* timer_delay ) {
    GTimeVal current_time;
    sice_timed_task* timed_task;

    timed_task = sice_peek_next_timed_task(timed_task_list);
    if (timed_task == NULL) {
        timer_delay = NULL;
    }
    else {
        g_get_current_time(&current_time);
        sice_set_time_difference(       timed_task->execution_time, 
                                        current_time, 
                                        timer_delay);
    }

    return timer_delay;
}

void 
run_signal_handler (    int             client_port_count, 
                        int*            client_ports, 
                        GQueue*         event_fifo, 
                        GQueue*         timed_task_list,
                        session_data    session) {
    fd_set master;
    fd_set read_fds;
    struct sockaddr_in native_addr[10];

    int fdmax = -1;
    int listeners[10];
    char buf[256];
    int nbytes;
    int i,j,new_connection,socket_interrupt,fromlen,retval;
    int round = 1;

    struct timeval timer_delay;
    struct timeval* timer_delay_pointer;
    sice_timed_task* timed_task;

    struct sockaddr_in from;
    struct sockaddr_in tmp_sockaddr_in;
    int initial_trial_socket;
 
    FD_ZERO (&master);
    FD_ZERO (&read_fds);

    for ( j = 0; j < client_port_count ; j++ ) {
        /* retval = create_socket(client_ports[j], &native_addr[j], &listeners[j]); */
        initial_trial_socket = client_ports[j];

        printf("Jump position %d\n", j);
        retval = create_any_socket(&initial_trial_socket, &client_ports[j],  &native_addr[j], &listeners[j]);
        if (retval != 0) {
          printf("SOCKET LUONTI EI ONNISTUNUT: %d\n", client_ports[j]);
        }

        FD_SET (listeners[j], &master);
        if (listeners[j] > fdmax) {
            fdmax = listeners[j];
        }
    }

    for (;/* forerver */;) {
        read_fds = master;  /* copy it */
        timer_delay_pointer = set_timed_task_list_delay(timed_task_list, &timer_delay);

        if (timer_delay_pointer != NULL) 
            printf ("Select blocking --- starts, maximum blocking time %d sec %d msec \n", 
                    (int) timer_delay.tv_sec, (int) timer_delay.tv_usec / 1000);
        else
            printf ("Select blocking --- starts, maximum blocking time indefinite \n");

        if (select (fdmax + 1, &read_fds, NULL, NULL, timer_delay_pointer) == -1) {
            perror ("select");
            exit (1);
        }
        printf ("Select blocking --- ends, round %d \n", round);
        round++;

        /* run through the existing connections looking for data to read */
        socket_interrupt = 0;
        for ( i = 0 ; i <= fdmax; i++ ) {
            if (FD_ISSET ( i, &read_fds ) ) {
                socket_interrupt = 1;
                new_connection = 0;
                for ( j = 0; j < client_port_count; j++ ) {
                    if (i == listeners[j]) { 
                        /* If data is sent to these sockets, it's automaticalluy new connection */
                        new_connection = 1; 
                        /* If data is sent to existing connection,
                           filedescriptor i is not among listeners[] */
                    }
                }
                /* receiving data */
                fromlen = sizeof(struct sockaddr_in);
                nbytes = recvfrom(i, buf, sizeof (buf), 0, (struct sockaddr *) &from, &fromlen);
                if (nbytes == -1) {
                    printf("ERROR in RECEIVE!\n");
                }

                tmp_sockaddr_in = (struct sockaddr_in) from;
                printf("RECEIVE PORT: %d\n", ntohs(tmp_sockaddr_in.sin_port));

                buf[nbytes] = 0;
                /* Terminating string */
                printf("Receiving something! (maybe): %s\n", buf);
                /* sending data */
                retval= sendto(i, buf, nbytes, 0,(struct sockaddr *) &from,fromlen);
                if (retval == -1) {
                    printf("ERROR in SEND!\n");
                }
            }
        }
        if (socket_interrupt == 1) {
            printf("SOCKET TRIGGER\n");
        }
        else {
            printf("TIMER TRIGGER\n");
            timed_task = sice_get_next_timed_task(timed_task_list); /* TODO: free mem */
            printf("Timed task: %s\n", (char*) timed_task->user_data);
        }
        sice_run_main_state_machine(event_fifo);
    }
}
           
/* _SICE_SIGNAL_HANDLER_C_ */
