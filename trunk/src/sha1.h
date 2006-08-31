/* Copyright (C) 2006  Movial Oy
 * authors:     ville.syrjala@movial.fi
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

#ifndef _SHA1_H_
#define _SHA1_H_

/**
 * hmac_sha1:
 * @key: secret key
 * @key_len: key length in bytes
 * @msg: message
 * @msg_len: message length in bytes
 * @hmac: calculated hmac
 *
 * Calculate HMAC-SHA1 using @message and @key.
 * The result is stored in @hmac, which must be able to hold 20 bytes.
 */
void 
hmac_sha1 (     const void      *key, 
                int             key_len,
	        const void      *message, int message_len,
	        void            *hmac);

#endif /* _SHA1_H_ */
