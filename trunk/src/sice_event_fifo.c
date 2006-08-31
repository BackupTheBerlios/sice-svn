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

#include "sice_event_fifo.h"
#include "../include/sice_types.h"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>    // sizeof

void     /* have to always use sice_add_new_event */
sice_set_event (         GQueue*        sice_event_fifo,
                         sice_event*    event ) {
   /* if ( sice_event_fifo == NULL ) exit */
    g_queue_push_tail ( sice_event_fifo, event );
}

GQueue*  
sice_initialize_event_fifo() {     
    return g_queue_new(); 
};

void
sice_delete_event_fifo (  GQueue* event_fifo ) {  
    /*  check if queue is empty and remove it */
    if ( g_queue_is_empty( event_fifo ) )
        g_queue_free( event_fifo );
    /*
     * else return ERROR ???? 
     */
}

/**********************************
 * sice_add_new_event - add new event to event fifo
 * ********************************/
void
sice_add_new_event (    GQueue*         sice_event_fifo,
//                        sice_event*      event,
                        E_EVENT_TYPE    ev_type,
                        gpointer        data ) {  
 
    /* check if buffer exist - if not, exit */
    if ( !sice_event_fifo ) {
        printf ("\nERROR: try to initialize event fifo before %s \n\n",__FUNCTION__);
        exit ( -1 );
    }
    /* allocate mem to event */
    sice_event*  event = (sice_event*)g_malloc0( sizeof( struct _sice_event ) );

    /* check success and set data to the event */
    if ( !event ) {
        /* TODO:error handling here */
    }
    else { 
        event->event_type = ev_type;
        event->user_data = data ;
    }
    /* push event to the queue */
    sice_set_event( sice_event_fifo, event );
}

void
sice_rem_event (         GQueue*         sice_event_fifo,
                        sice_event*      event ) {    
/* maybe we do not need this ?!?!?!? */
}

sice_event* 
sice_get_event (         GQueue*         sice_event_fifo ) {  
    /* if ( sice_event_fifo == NULL ) exit */     
    return g_queue_pop_head( sice_event_fifo );                   
}

/* static void */    /* have to always use sice_add_new_event */
/* sice_set_event (         GQueue*         sice_event_fifo,
                        sice_event*      event ) {  */
   /* if ( sice_event_fifo == NULL ) exit */
/*    g_queue_push_tail ( sice_event_fifo, event ); */
/* } */
/*
void
sice_sort_event (        GQueue*         sice_event_fifo ) {

    return sice_event_fifo;
}
*/


/* _SICE_EVENT_BUFFER_C_ */
