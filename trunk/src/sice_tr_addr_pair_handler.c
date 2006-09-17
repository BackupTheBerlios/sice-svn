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
#include "sice_tr_addr_pair_handler.h"

#include <glib.h>


/* static function's for state machine ***********************************/
static E_TAP_STATE
sice_tap_waiting         (       GQueue*                         event_fifo,
                                sice_transport_address_pair*     t_addr_pair,
                                sice_stun_message*               stun_msg );

static E_TAP_STATE
sice_tap_testing         (       GQueue*                         event_fifo,
                                sice_transport_address_pair*     t_addr_pair,
                                sice_stun_message*               stun_msg );

static E_TAP_STATE
sice_tap_recv_valid      (       GQueue*                         event_fifo,
                                sice_transport_address_pair*     t_addr_pair,
                                sice_stun_message*               stun_msg );        

static E_TAP_STATE
sice_tap_send_valid      (       GQueue*                         event_fifo,
                                sice_transport_address_pair*     t_addr_pair,
                                sice_stun_message*               stun_msg );
static E_TAP_STATE
sice_tap_valid           (       GQueue*                         event_fifo,
                                sice_transport_address_pair*     t_addr_pair,
                                sice_stun_message*               stun_msg );

static E_TAP_STATE
sice_tap_invalid         (       GQueue*                         event_fifo,
                                sice_transport_address_pair*     t_addr_pair,
                                sice_stun_message*               stun_msg );
/*************************************************************************/




void
sice_run_transport_address_pair_state_machine(   GQueue*                         event_fifo,
                                                sice_transport_address_pair*     t_addr_pair,
                                                sice_stun_message*               stun_msg ) {
    
    switch ( t_addr_pair->state ) {
        
        case WAITING:
        /* next state can be :recv_valid, send_valid, testing, invalid */
                t_addr_pair->state = sice_tap_waiting ( event_fifo, t_addr_pair, stun_msg );
            break;
        
        case TESTING:
        /* next state can be: recv_valid, send_valid, invalid */
                t_addr_pair->state = sice_tap_testing ( event_fifo, t_addr_pair, stun_msg );
            break;
        
        case RECV_VALID:
        /* next state can be: invalid, valid  [timer trigger]*/
                t_addr_pair->state = sice_tap_recv_valid ( event_fifo, t_addr_pair, stun_msg );
            break;
        
        case SEND_VALID:
        /* next state can be: invalid, valid [timer trigger]*/
                t_addr_pair->state = sice_tap_send_valid ( event_fifo, t_addr_pair, stun_msg );
            break;
        
        case VALID:
        /* next state can be: invalid [timer trigger]*/
                t_addr_pair->state = sice_tap_valid ( event_fifo, t_addr_pair, stun_msg );
            break;
        
        case INVALID:
        /* no next state : */
                t_addr_pair->state = sice_tap_invalid ( event_fifo, t_addr_pair, stun_msg );
            break;
        
        default :
            break;
    }

}


static E_TAP_TYPE
sice_tap_waiting (       GQueue*                         event_fifo,
                        sice_transport_address_pair*     t_addr_pair,
                        sice_stun_message*               stun_msg ) {
    /* In here we wait one of three events :
     *  - chance to send a Binding request
     *  - receipt of Binding Request 
     *  - receipt of Binding Response 
     */   
    
    
    /* take current state */
    E_TAP_STATE current_state = t_addr_pair->state;    

    if () {

        return RECV_VALID;
    }

    else if () {

        return SEND_VALID;
    }
    
    else if () {
    /* "selected" event cause */
    
        return TESTING;
    }


    else
        return INVALID;
}


static E_TAP_TYPE
sice_tap_testing (       GQueue*                         event_fifo,
                        sice_transport_address_pair*     t_addr_pair,
                        sice_stun_message*               stun_msg ) {
    
    /* The agent then sends a connectivity check using 
     * a STUN Binding Request
     */
    
    /* take current state */
    E_TAP_STATE current_state = t_addr_pair->state;    
    
    if () {

        return RECV_VALID;
    }

    else if () {

        return SEND_VALID;
    }
    
    else 
        return INVALID;
}

static E_TAP_TYPE
sice_tap_recv_valid (    GQueue*                         event_fifo,
                        sice_transport_address_pair*     t_addr_pair,
                        sice_stun_message*               stun_msg ) {
    /* we know we can receive data from the peer */

    /* take current state */
    E_TAP_STATE current_state = t_addr_pair->state;    
    
    if () {

        return VALID;
    }

    else
        return INVALID;
}


static E_TAP_TYPE
sice_tap_send_valid (    GQueue*                         event_fifo,
                        sice_transport_address_pair*     t_addr_pair,
                        sice_stun_message*               stun_msg ) {
    /* we know we can send data to the peer*/
    /* take current state */
    E_TAP_STATE current_state = t_addr_pair->state;    
    
    if () {

        return VALID;
    }

    else
        return INVALID;
}


static E_TAP_TYPE
sice_tap_valid (         GQueue*                         event_fifo,
                        sice_transport_address_pair*     t_addr_pair,
                        sice_stun_message*               stun_msg ) {
    
    /* take current state */
    E_TAP_STATE current_state = t_addr_pair->state;    
    
    if () {

        return INVALID;
    }

    else
        return VALID;
}


static E_TAP_TYPE
sice_tap_invalid (       GQueue*                         event_fifo,
                        sice_transport_address_pair*     t_addr_pair,
                        sice_stun_message*               stun_msg ) {
    
    
        return INVALID;
}


 /* _SICE_INTERFACE_C_ */
