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

#include "../include/sice_types.h"
#include "sice_common.h"

#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <stdio.h>

/** static's **/
static gint
sice_cmp_candidates(	sice_candidate*  C1,
                       	sice_candidate*  C2,
                       	gpointer        user_data );

static gint
sice_cmp_transport_address_pairs(        sice_candidate*  C1,
                                        sice_candidate*  C2,
                                        gpointer        user_data );

/*****************/

GQueue*
sice_init_candidate_pairs() {
    return g_queue_new();
}



/* 
 * generate candidate pairs, N native and R remote candidates produces N * R pairs
 * N and R came from client.
 */
void
sice_generate_candidate_pairs(   GQueue* sice_candidate_pairs,
                                GList*  sice_native_candidates,
                                GList*  sice_remote_candidates ) {
    
    /* for generating pairs */
    GList *current_native, *current_remote;
    /* for checking if pairs components do have a pair */
    GList *native_component, *remote_component;
    
    
    current_native = g_list_first ( sice_native_candidates );

    while ( current_native ) {
       
        current_remote = g_list_first ( sice_remote_candidates ); 
        
        while ( current_remote ) {
            
            sice_candidate_pair *cp;
            /* allocate and fill candidate pair */
            cp = ( sice_candidate_pair* )g_malloc0( sizeof( struct _sice_candidate_pair ));
            cp->remote_candidate = current_remote->data; 
            cp->native_candidate = current_native->data; 
            
            /* check if component do have a pair */
            native_component = g_list_first( (GList*) &(cp->native_candidate->components) );
            remote_component = g_list_first( (GList*) &(cp->remote_candidate->components) );
            
            /* while native or remote do have a component */        
            while ( native_component || remote_component ) {
               /* N = native, R = remote */ 
                sice_component *N,*R;
                N = ( sice_component* )native_component->data;
                R = ( sice_component* )remote_component->data;
               /* checking if they have same id, so they have same component quantity */
                if ( R->id == N->id ) {
                     /* have to be initialized: FALSE   */
                    R->is_paired = TRUE;
                    N->is_paired = TRUE;
                }
                /* take address of next component */
                native_component = g_list_next ( native_component );    
                remote_component = g_list_next ( remote_component );    
            }    

            /* push to queue */
            g_queue_push_head( sice_candidate_pairs, cp ) ;
            current_remote = g_list_next( current_remote ); 
        }
        
        current_native = g_list_next( current_native );
    }
    
}

/*
 * Order candidate pairs. Take into account q-value, candidate IDs etc.
 */
void
sice_order_candidates(      GQueue*  sice_candidates     ) {
    /* sort queue, use function sice_cmp_candidates */
    g_queue_sort ( sice_candidates, (GCompareDataFunc)sice_cmp_candidates, NULL );
   
    /* set sequence number for each candidate */
    guint seq_id = 0;
    GList* candidate_list = NULL;
    sice_candidate* candidate = NULL;
    candidate_list = ( GList* )g_queue_peek_head ( sice_candidates );
    candidate = ( sice_candidate* )g_list_first( candidate_list );
    while ( candidate ) {
        candidate->sequence_id = seq_id;
        seq_id++;
        candidate = ( sice_candidate* )g_list_next ( candidate_list );
    } 
}

 /*
  * Get transport address pair check ordered list.
  * This has approximately same order and content than ordered candidate pairs.
  * However, pairs related to operating candidate are on top, and that redundant
  * entries are filtered out.
  */
void 
sice_generate_transport_address_pair_check_ordered_list( GQueue*  sice_candidate_pairs ) {

   g_queue_sort ( sice_candidate_pairs, (GCompareDataFunc)sice_cmp_transport_address_pairs, NULL );

} 

static gint
sice_cmp_candidates(             sice_candidate*  C1,
                                sice_candidate*  C2,
                                gpointer        user_data ) {
    /*highest qvalues comes first, if equal sorted by candidate_id using reverse ASCII */
    if (C1->q_value > C2->q_value )
        return 1;
    if (C1->q_value < C2->q_value )
        return -1;
    else /* reverse ASCII */
        return strcmp( C1->id, C2->id );
}
 
static gint
sice_cmp_transport_address_pairs(        sice_candidate*  C1,
                                        sice_candidate*  C2,
                                        gpointer        user_data ) {
    /* if candidate is operating, move to first  */
    if ( C1->is_operating && !(C2->is_operating) )
        return 1;
    if ( C2->is_operating && !(C1->is_operating) )
        return -1;
    else {      /* if candidate is not operating, resort first by lowest sequence_id*/
            if ( C1->sequence_id < C2->sequence_id )
                return 1;
            if ( C2->sequence_id < C1->sequence_id )
                return -1;
            else /* then by reverse ASCII */
                return strcmp (C1->id, C2->id );
            
    }        
    /* never reach this point */
    return 0;
}
 


/* _SICE_COMMON_C_ */
