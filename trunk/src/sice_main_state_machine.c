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

#include "../include/sice_types.h"
#include "sice_event_fifo.h"

#include "sice_main_state_machine.h"

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

void 
sice_run_main_state_machine(    GQueue* event_fifo ) {
 /* NOTE: this is only for temporary testing purpose 
  * but main idea is ok
  */ 
    sice_event* popped_event;

    // pop first event from queue
    popped_event = sice_get_event( event_fifo );
    while( popped_event != NULL ) {

        switch ( (*popped_event).event_type ) {

            case TEST_PRINT_V1:
                printf("Main state machine executes TEST_PRINT_V1 event, prints msg: %s\n",
                        (char*) (*popped_event).user_data );
                break;

            case TEST_PRINT_V2:
                printf("Main state machine executes TEST_PRINT_V2 event, prints msg: %s\n", 
                        (char*) (*popped_event).user_data );
                break;

            case TEST_EV0:
                printf("Main state doesn't currently support TEST_EV0 event\n");
                break;

            default:
                printf("Main state machine has no support for this event type!\n");
                break;
        }

        free( (*popped_event).user_data );
        free( popped_event );
        printf("Event queue has %d more events to be processed\n",
                g_queue_get_length( event_fifo ) );
        popped_event = sice_get_event(event_fifo);
    };
}

 /* _SICE_MAIN_STATE_MACHINE_C_ */
