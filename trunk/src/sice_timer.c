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
#include <stdlib.h>     
#include "sice_timer.h"
#include <stdio.h>
#include "sice_event_fifo.h"
 
/*******   for random number generator ********/
static GRand            *rand_val;

/* test variable 'candidate_pair_count'*/
const static guint      candidate_pair_count = 1;

/***** static's ****************************************************/
static void
sice_set_timed_task (    GQueue*                sice_timed_task_list,
                         sice_timed_task*       timed_task );

static gint
sice_cmp_timed_task      (  sice_timed_task*    A,
                            sice_timed_task*    B,
                            gpointer            user_data );

/*******************************************************************/


GQueue*
sice_initialize_timed_task_list() {
    /* g_rand_new ???  */
    rand_val = g_rand_new();
    return g_queue_new();
}

/* Note: no negative time differences (== past) are tolerated, those are interpreted as zero (== now) for the 
 * purposes of select operation. */
void 
sice_set_time_difference (      GTimeVal            later_moment, 
                                GTimeVal            earlier_moment, 
                                struct timeval*     time_difference ) {
    long nsec;
    long usec_diff;
    long sec_diff;

    sec_diff = later_moment.tv_sec - earlier_moment.tv_sec;
    usec_diff = later_moment.tv_usec - earlier_moment.tv_usec;

    /* remove negative usecs */
    if ( usec_diff < 0 ) {
        nsec = -usec_diff / 1000000 + 1;
        sec_diff = sec_diff - nsec;
        usec_diff = usec_diff + 1000000 * nsec;
    }

    /* remove multiplicites of usecs */
    if ( usec_diff > 1000000 ) {
        nsec = usec_diff / 1000000;
        sec_diff = sec_diff + nsec;
        usec_diff = usec_diff - nsec * 1000000;
    }
    /* set result,but NEGATIVE (==past) times are interpreted as zero (==now) */
    if ( sec_diff < 0 ) {
        (*time_difference).tv_sec = 0;
        (*time_difference).tv_usec = 0;
    } else {
        (*time_difference).tv_sec = sec_diff;
        (*time_difference).tv_usec = usec_diff;
    }
}

void
sice_add_timed_task (           GQueue*                 sice_timed_task_list,
                                sice_timed_task*        timed_task,
                                E_TIMED_TASK_TYPE       t_type,
                                E_EVENT_TYPE            e_type,
                                gpointer                user_data ) {

    if ( !sice_timed_task_list ) {
        exit ( -1 );
    }
    /* allocate mem to task */
    timed_task = (sice_timed_task*)g_malloc0( sizeof( struct _sice_timed_task ) );
     
    /* check success and set data to the event */
    if ( !timed_task ) {
        exit ( -1 );
        /* TODO:better error handling here */
    }
    else {
        g_get_current_time( &(timed_task->execution_time) );

        /*  generate expiration time depending to the type of the task */
        switch( t_type ) {
            case TYPE_BINDING_REQUEST:
                g_time_val_add( &(timed_task->execution_time),
                                candidate_pair_count*SICE_CONST_DELAY*
                                g_rand_double_range(    rand_val,
                                                        SICE_MSG_DELAY_BEGIN, 
                                                        SICE_MSG_DELAY_END ) );
                break;

            case TYPE_MAX_CONN_CHECK_WAIT:
                g_time_val_add( &(timed_task->execution_time),
                                SICE_MAX_CONN_CHECK_WAIT );
                break;

            case TYPE_MAX_ANSWER_WAIT:
                g_time_val_add( &(timed_task->execution_time),
                                SICE_MAX_ANSWER_WAIT );
                break;

            case TYPE_KEEP_ALIVE:
                 g_time_val_add( &(timed_task->execution_time),
                                 SICE_WAIT_4_KEEP_ALIVE );
                break;
            case TYPE_GENERATE_EVENT:
                  g_time_val_add( &(timed_task->execution_time),
                                  SICE_EVENT_TYPE );
               
                break;
            default:
                break;
        }
 
        timed_task->task_type = t_type;
        timed_task->user_data = user_data ;
    
    /* push task to the queue */
    sice_set_timed_task( sice_timed_task_list, timed_task );

    }
}

void
sice_add_timed_task_with_delay (        GQueue*             sice_timed_task_list,
                                        guint               delay_ms,
                                        E_TIMED_TASK_TYPE   type,
                                        void*               user_data ) {

    sice_timed_task* timed_task;

    if ( !sice_timed_task_list ) {
        exit ( -1 );
    }
    /* allocate mem to task */
    timed_task = (sice_timed_task*)g_malloc0( sizeof( struct _sice_timed_task ) );

    /* check success and set data to the event */
    if ( !timed_task ) {
        exit ( -1 );
        /* TODO:better error handling here */
    } else {
        g_get_current_time( &(timed_task->execution_time) );
        g_time_val_add( &(timed_task->execution_time), delay_ms * 1000);

        timed_task->task_type = type;
        timed_task->user_data = user_data ;

        /* push task to the queue */
        sice_set_timed_task( sice_timed_task_list, timed_task );
    }
}

sice_timed_task*
sice_get_next_timed_task (       GQueue*                 sice_timed_task_list ) {
    /* sort and return next task */
    g_queue_sort ( sice_timed_task_list,(GCompareDataFunc)sice_cmp_timed_task, NULL );
    return g_queue_pop_head ( sice_timed_task_list );
}

sice_timed_task*
sice_peek_next_timed_task (       GQueue*                 sice_timed_task_list ) {
    /* sort and return next task */
    g_queue_sort ( sice_timed_task_list,(GCompareDataFunc)sice_cmp_timed_task, NULL );
    return g_queue_peek_head ( sice_timed_task_list );
}

/* static, needed only sice_add_timed_task */
static void
sice_set_timed_task(            GQueue*                 sice_timed_task_list,
                                sice_timed_task*         timed_task ) {

    g_queue_push_head( sice_timed_task_list, timed_task );
}    

static gint     /* ok */
sice_cmp_timed_task      (        sice_timed_task*               A,
                                  sice_timed_task*               B,
                                  gpointer                      user_data ) {
                                
   if (( A->execution_time.tv_sec == B->execution_time.tv_sec ) && 
       ( A->execution_time.tv_usec < B->execution_time.tv_usec )) 
       return -1;
    
   if (( A->execution_time.tv_sec == B->execution_time.tv_sec ) && 
       ( A->execution_time.tv_usec > B->execution_time.tv_usec )) 
       return 1;

   if ( A->execution_time.tv_sec < B->execution_time.tv_sec )
       return -1;
   if ( A->execution_time.tv_sec > B->execution_time.tv_sec ) 
       return 1;
   else 
       return 0;
}

/* _SICE_TIMER_C_ */
