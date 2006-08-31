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

#ifndef _SICE_INTERFACE_H_
#define _SICE_INTERFACE_H_


#include "sice_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/*! enumerated list indicates type of callback */
typedef enum _sice_callback_type         E_CALLBACK_TYPE;

/*! ice callback enums */
enum    _sice_callback_type  {
            
            NATIVE_GANDIDATES_GATHERED  = 0     , 
            NEW_CANDIDATE_FOUND                 ,
            NEW_ACTIVE_CANDIDATE_PAIR           ,
            CONNECTION_STATE                    ,
            ERROR_OCCURRED                      
};

/*! struct _sice_media_conn_state includes some usefull information from media connection properties */
typedef struct _sice_media_conn_state          sice_conn_state;
struct  _sice_media_conn_state {
    
    unsigned short int          send_recv_capable;      /*  0 means false*/
    unsigned short int          send_recv_on;           /*  0 means false*/
    unsigned short int          error_code;    

};

/*!
       Only this callback call is possible. 
       - session_id points operations to the correct session
       - cp_type tells wich kind of callback is coming
       - data is pointed by user_data
 
       When cp_type is NATIVE_CANDIDATES_GATHERED, user_data's type is a GList, where each 
       node has as data's type sice_candidate.
               
       When cp_type is NEW_CANDIDATE_FOUND, user_data's type is a sice_candidate.
 
       When cp_type is NEW_ACTIVE_CANDIDATE_PAIR, user_data's type is sice_candidate_pair, 
       which has pointers typed sice_candidate as native_candidate and remote_candidate.
 
       When cp_type is CONNECTION_STATE, user_data's type is a sice_conn_state.
 
       When cp_type is ERROR_OCCURRED, user_data's type is |( not defined yet )|
 */
typedef void ( *sice_callback )          (      int                     session_id,
                                                E_CALLBACK_TYPE         cb_type,
                                                void*                   user_data );
/*!

 */


/*!
       sice_initialize -function binds callback function to correct session 
       using session_id.
 */
int
sice_initialize                  (      int                     session_id,
                                        int                     client_port_count,
                                        int*                    client_ports,
                                        sice_callback            callback );

/*! finalize / stop single session pointed by session_id */
int    
sice_finalize                    (       int                     session_id );
/*! start gathering native candidate's  */
int    
sice_gather_native_candidates    (       int                     session_id );

/*! 
      sice_set_remote_cadidates -function sets information of remote candidates,
      which has received via signaling, to the ice component. 
      Each node of GList has as data's type sice_candidate.
 */
int
sice_set_remote_candidates       (      int                     session_id,
                                        GList                   sice_remote_candidates );

/*!
      sice_set_active_cadidate_pair -function takes session_id and pointer to
      sice_candidate_pair , which has pointers to remote and native candidate     
 */

int
sice_set_active_candidate_pair   (      int                     session_id,
                                        sice_candidate_pair*     sice_candidate_pair );

/*! add single remote candidate to list of remote candidates */
int 
sice_add_remote_candidate        (      int                     session_id,
                                        sice_candidate*          sice_remote_candidate );

/*! remove single remote candidate from list of remote candidates*/
int  
sice_del_remote_candidate        (       int                     session_id,
                                        sice_candidate*          sice_remote_candidate );

/*!  change / set media connection state   */
int   
sice_set_media_conn_state        (       int                     session_id,
                                        sice_conn_state          conn_state      );



#ifdef __cplusplus
}
#endif

#endif /* _SICE_INTERFACE_H_ */
