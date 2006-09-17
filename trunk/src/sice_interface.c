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
#include "../include/sice_interface.h"
#include "sice_signal_handler.h"

int
sice_initialize                  (      int                     session_id,
                                        int                     port_count,
                                        int*                    client_ports,
                                        int*                    library_ports,
                                        sice_callback           callback ) { 
  return signal_handler_initialize ( session_id, port_count, client_ports, library_ports, callback);

}

int    /* finalize / stop one session pointed by session_id */
sice_finalize                    (       int                    session_id ) {



    return 0;
}

int    /* set information of remote candidates to ice component */
sice_set_remote_candidates       (       int                     session_id,
                                        GList                   sice_remote_candidates ) {


    return 0;
}

int   
sice_set_active_candidate_pair   (       int                     session_id,
                                        sice_candidate_pair*     sice_candidate_pair ) {


    return 0;
}




int    /* add single remote candidate to list of remote candidates */
sice_add_remote_candidate        (       int                     session_id,
                                        sice_candidate*          sice_remote_candidate ) {



    return 0;
}

int    /* remove single remote candidate from list of remote candidates*/
sice_del_remote_candidate        (       int                     session_id,
                                        sice_candidate*          sice_remote_candidate ) {


    return 0;
}

int    /*  change/set media connection state   */
sice_set_media_conn_state        (       int                     session_id,
                                        sice_conn_state          conn_state      ) {



    return 0;
}


 /* _SICE_INTERFACE_C_ */
