
/* Copyright (C) 2006  Movial Oy
 * authors: 	re@welho.com
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

#ifndef _SICE_TYPES_H_
#define _SICE_TYPES_H_

#include <glib.h>




#ifdef __cplusplus
extern "C" {
#endif

/*      Transport Address Pair State*/
typedef enum TAP_STATE          E_TAP_STATE;
enum TAP_STATE { 
            WAITING,
            TESTING, 
            RECV_VALID,
            SEND_VALID,
            VALID,
            INVALID
    };

typedef enum COMPONENT_TYPE     E_COMPONENT_TYPE;
enum COMPONENT_TYPE {
            LOCAL,
            SERVER,
            REFLEXIVE,
            PEER_REFLEXIVE,
            RELAYED
    };

typedef enum CANDIDATE_TYPE     E_CANDIDATE_TYPE;
enum CANDIDATE_TYPE {
            LOCAL_CANDIDATE,
            SERVER_REFLEXIVE_CANDIDATE,
            PEER_REFLEXIVE_CANDIDATE,
            RELAYED_CANDIDATE
    };

typedef enum PRIORITY_TYPE      E_PRIORITY_TYPE;
enum PRIORITY_TYPE { 
            INTERMEDIARY_CANDIDATE_PRIORITY,
            RESPONSE_DELAY_CANDIDATE_PRIORITY 
};

/*      Candidate Pair State    */
typedef enum CP_STATE           E_CP_STATE;
enum CP_STATE {
        CP_WAITING,
        CP_TESTING,
        CP_RECV_VALID,
        CP_SEND_VALID,
        CP_VALID,
        CP_INVALID,
        CP_INDETERMINATE 
};

/*      Candidate State    */
typedef enum CAND_STATE         E_CAND_STATE;
enum CAND_STATE {
        VALID_CAND,
        INVALID_CAND,
        INDETERMINATE_CAND,
};


extern GList       sice_native_candidates;
extern GList       sice_remote_candidates;
extern GQueue      sice_candidate_pairs;
extern GList       sice_servers;
extern GList       sice_transport_address_pairs;
extern GList       sice_candidate_priorities;

typedef struct _sice_candidate                   sice_candidate;
typedef struct _sice_component                   sice_component;
typedef struct _sice_ext_server                  sice_ext_server;
typedef struct _sice_stun                        sice_stun;
typedef struct _sice_turn                        sice_turn;
typedef struct _sice_candidate_pair              sice_candidate_pair;
typedef struct _sice_transport_address_pair      sice_transport_address_pair;
typedef struct _sice_signal_address              sice_signal_address;
typedef struct _sice_stun_message                sice_stun_message;
typedef struct _sice_stun_message_header         message_header;
typedef struct _sice_ipv4_address                addr4;



struct _sice_ipv4_address {
    guint16   port;     // unsigned short 
    guint32   addr;     // unsigned int
};

struct _sice_candidate {
    E_CANDIDATE_TYPE    type;
    gchar*              network_interface;
    GList*              components;
    gchar*              id; 
    guint               state;
    gfloat              q_value; 
    gboolean            is_operating;
    guint               sequence_id;

};


struct _sice_component {
    
    E_COMPONENT_TYPE    type;
    guint               id;
    /* native and remote candidate components have a pair */
    gboolean	        is_paired; // have to init 0
    addr4               address;

   // gchar*       XOR-MAPPED-ADDRESS;

};


struct _sice_ext_server {
    
    sice_stun*          stun;
    sice_turn*          turn;
    addr4               address;

};


struct _sice_stun {

    gboolean            is_enabled;

};


struct _sice_turn {

    gboolean            is_enabled;

};

struct _sice_candidate_pair {

    E_CP_STATE          state;
    sice_candidate*     native_candidate;
    sice_candidate*     remote_candidate;

};

struct _sice_transport_address_pair {
    
    E_TAP_STATE         state;
    gchar*		        transport_address_pair_id;
    sice_candidate*     native_candidate; 
    sice_candidate*     remote_candidate;
    guint		        component_id;

};

#ifdef __cplusplus
}
#endif

#endif /* _SICE_TYPES_H_ */
