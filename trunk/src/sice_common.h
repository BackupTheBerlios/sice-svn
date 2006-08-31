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

#ifndef _SICE_COMMON_H_
#define _SICE_COMMON_H_

#include <glib.h>

#ifdef __cplusplus
extern "C"
#endif


GQueue*
sice_init_candidate_pairs();

/*
 * Form candidate pairs, N native and R remote candidates produces N * R pairs.
 */
void
sice_generate_candidate_pairs(   GQueue* sice_candidate_pairs,
                                GList*  sice_native_candidates,
                                GList*  sice_remote_candidates );
/*
 * Order candidate pairs. Take into account q-value, candidate IDs etc.
 */
void
sice_order_candidates(      GQueue*  sice_candidates     );

 /*
  * Get transport address pair check ordered list.
  * This has approximately same order and content than ordered candidate pairs.
  * However, pairs related to operating candidate are on top, and that redundant
  * entries are filtered out.
  */
void 
sice_generate_transport_address_pair_check_ordered_list( GQueue*  sice_candidate_pairs );


#ifdef __cplusplus
}
#endif

#endif /* _SICE_COMMON_H_ */

/* _SICE_COMMON_C_ */
