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

#ifndef _SICE_TIMER_H_
#define _SICE_TIMER_H_

#define G_USEC_PER_SEC          1000000 
#define SICE_CONST_DELAY         50000   // 50 ms
#define SICE_MSG_DELAY_BEGIN     0.7
#define SICE_MSG_DELAY_END       1.3
#define SICE_MAX_ANSWER_WAIT     5000000 //  5 sec in microsec
#define SICE_MAX_CONN_CHECK_WAIT 5000000
#define SICE_WAIT_4_KEEP_ALIVE   2000000 // 2 sec
#define SICE_EVENT_TYPE          50000   // 50 ms FIX:later

#include <glib.h>
#include <sys/time.h>
#include "sice_event_fifo.h"
#ifdef __cplusplus
extern "C" {
#endif
 
/* timed task types */

typedef enum TIMED_TASK_TYPE            E_TIMED_TASK_TYPE;
enum TIMED_TASK_TYPE {
        TYPE_BINDING_REQUEST=1,
        TYPE_CONNECTIVITY_CHECK,
        TYPE_MAX_CONN_CHECK_WAIT,
        TYPE_MAX_ANSWER_WAIT,
        TYPE_TEST_DELAY_V1, /* don't remove, in use */
        TYPE_KEEP_ALIVE,
        TYPE_GENERATE_EVENT,
        TYPE_TEST2,
        TYPE_TEST3,
        TYPE_TEST4
};


typedef struct _sice_timed_task          sice_timed_task;      
struct _sice_timed_task  {
         GTimeVal                       execution_time;
         enum TIMED_TASK_TYPE           task_type;
         gpointer                       user_data;
};

GQueue*
sice_initialize_timed_task_list();

void
sice_add_timed_task (            GQueue*                 sice_timed_task_list,
                                sice_timed_task*         timed_task,
                                E_TIMED_TASK_TYPE       t_type,
                                E_EVENT_TYPE            e_type,
                                gpointer                user_data );

void
sice_set_time_difference ( GTimeVal later_moment, GTimeVal earlier_moment, struct timeval* time_difference );

void
sice_add_timed_task_with_delay ( GQueue* sice_timed_task_list,
                                guint delay_ms,
                                E_TIMED_TASK_TYPE type,
                                gpointer user_data );
                                                                                                
sice_timed_task*
sice_get_next_timed_task (       GQueue*                 sice_timed_task_list );

sice_timed_task*
sice_peek_next_timed_task (       GQueue*                 sice_timed_task_list );


/********************************************************************'
static void
sice_set_timed_task (            GQueue*                 sice_timed_task_list,
                                sice_timed_task*         timed_task );

static gint
sice_cmp_timed_task      (       sice_timed_task*               A,
                                sice_timed_task*               B,
                                gpointer                user_data );
************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _SICE_TIMER_H_ */
