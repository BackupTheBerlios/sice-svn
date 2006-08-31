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

#ifndef _SICE_EVENT_FIFO_H_
#define _SICE_EVENT_FIFO_H_

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* enumerated list's */
typedef enum EVENT_TYPE                 E_EVENT_TYPE;
enum EVENT_TYPE {
        TEST_EV0,
        TEST_EV1,
        TEST_EV2,
        TEST_EV3,
        TEST_EV4,
        TEST_PRINT_V1,
        TEST_PRINT_V2
};

/* extern's */
extern GQueue                           sice_event_fifo;

/* typedef's */
typedef struct _sice_event               sice_event;

/* struct's */
struct _sice_event {
    E_EVENT_TYPE        event_type;
    gpointer            user_data;
};// _sice_event;

/* function prototypes */
GQueue* 
sice_initialize_event_fifo();

void
sice_delete_event_fifo(   GQueue*        sice_event_fifo );

void
sice_add_new_event (     GQueue*         sice_event_fifo,
//no need for this ??   sice_event*      event,
                        E_EVENT_TYPE    type,
                        gpointer        data );

void
sice_rem_event (         GQueue*         sice_event_fifo,
                        sice_event*      event );

sice_event*
sice_get_event (         GQueue*         sice_event_fifo );

void
sice_set_event (        GQueue*         sice_event_fifo,
                       sice_event*      event );
void
sice_sort_event (        GQueue*         sice_event_fifo );

#ifdef __cplusplus
}
#endif

#endif /* _SICE_EVENT_FIFO_H_ */
