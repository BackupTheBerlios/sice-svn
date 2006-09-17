/* Copyright (C) 2006  Movial Oy
 * authors:     re@welho.com
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

#ifndef _SICE_STUN_H_
#define _SICE_STUN_H_

#include "../include/sice_types.h"


#ifdef __cplusplus
extern "C" {
#endif

#define         FAMILY_IPv4             0x01
#define         MAX_MEDIA_RELAYS        250
#define         MAX_STRING              256
#define         MAX_UNKNOWN_ATTR         8
#define         MAX_MSG_SIZE            2048

typedef struct { unsigned char oct[16]; }       uint128;


typedef enum _NAT_TYPE          E_NAT_T;
enum _NAT_TYPE {
            NAT_NO_ACCESS,
            NAT_NONE,
            NAT_FULL_CONE,
            NAT_SYMMETRIC,
            NAT_SYMMETRIC_FIREWALL,
            NAT_RESTRICTED,
            NAT_PORT_RESTRICTED,
            NAT_UNKNOWN
};

typedef enum _STUN_MESSAGE_TYPES        E_STUN_MSG_TYPE;
enum _STUN_MESSAGE_TYPES     {
/*! RFC 3489 */	
            BINDING_REQUEST             =       0x0001  ,       // 
            BINDING_RESPONSE            =       0x0101  ,       // 
            BINDING_ERROR_RESPONSE      =       0x0111  ,       // 
            SHARED_SECRET_REQUEST       =       0x0002  ,       //
            SHARED_SECRET_RESPONSE      =       0x0102  ,       //
            SHARED_ERROR_RESPONSE       =       0x0112          //
};

typedef enum _STUN_MESSAGE_ATTRIBUTES   E_STUN_MSG_ATTR;
enum _STUN_MESSAGE_ATTRIBUTES   {
/*! RFC 3489 */
            MAPPED_ADDRESS              =       0x0001 ,        /*! bits 0-7 are ignored -> 8-15 indicates family and 16-31 indicates port = total = IPv4 = 32 bits */
            RESPONSE_ADDRESS            =       0x0002 ,        /*! where to responce to. syntax is identical to MAPPED_ADDRESS*/
            CHANGE_REQUEST              =       0x0003 ,        //
            SOURCE_ADDRESS              =       0x0004 ,        //
            CHANGED_ADDRESS             =       0x0005 ,        //
            USERNAME                    =       0x0006 ,        //
            PASSWORD                    =       0x0007 ,        //
            MESSAGE_INTEGRITY           =       0x0008 ,        //
            ERROR_CODE                  =       0x0009 ,        //
            UNKNOWN_ATTRIBUTES          =       0x000a ,        //
            REFLECTED_FROM              =       0x000b          //
};

typedef enum _STUN_RESPONSE_CODE        E_STUN_RESPONSE_CODE;
enum   _STUN_RESPONSE_CODE      {
/*! RFC 3489 */
            BAD_REQUEST                 =       400 ,           /*! request was malformed */
            UNAUTHORIZED                =       401 ,           /*! binding req did not contain MESSAGE_INTEGRITY*/
            UNKNOWN_ATTRIBUTE           =       420 ,           
            STALE_CREDENTIALS           =       430 ,           /*! client should obtain new shared secret key and try again*/
            INTEGRITY_CHECK_FAILURE     =       431 ,           /*! sign of potential attack, or bad client implemetation*/
            UNAME_MISSING               =       432 ,
            USE_TLS                     =       433 ,
            SERVER_ERROR                =       500 ,           /*! client should try again */
            GLOBAL_FAILURE              =       600 ,           /*! client should not retry */
};

typedef struct _sice_stun_message_header         stun_message_header;
struct _sice_stun_message_header {

        unsigned short  message_type;
        unsigned short  message_length;
        uint128         message_id;         
};

typedef struct _sice_stun_attribute_address       attr_addr4;
struct _sice_stun_attribute_address {

    unsigned char       pad;
    unsigned char       ip_family;
    addr4               ip;
};

    
typedef struct _sice_stun_attribute_header       attribute_header;
struct _sice_stun_attribute_header {

    unsigned short     attribute_type;
    unsigned short     attribute_len;
};

typedef struct _sice_stun_media_relay            stun_relay;
struct _sice_stun_media_relay {
   
    int         port;
    int         fd;
    addr4       NAT_address;
    //time_t      expire_time;
};

typedef struct _sice_stun_server_info            server_info;
struct  _sice_stun_server_info {

    addr4       my_address;
    addr4       alt_address;
    int         my_fd;
    int         alt_port_fd;
    int         alt_ip_fd;
    int         alt_ip_port_fd;
    int         is_relay;       /*! 0 if media relaying is not to be done */
    stun_relay  relays[MAX_MEDIA_RELAYS];
};

typedef struct _sice_stun_attribute_integrity            attr_integrity;
struct _sice_stun_attribute_integrity {

        unsigned char           sha1_hash[20];       

};


typedef struct _sice_stun_attr_change_req                attr_change_req;
struct _sice_stun_attr_change_req {
        unsigned int    val;
};

typedef struct _sice_stun_attr_error                     attr_error;
struct _sice_stun_attr_error {
        unsigned short          pad; // all 0
        unsigned char           err_class;
        unsigned char           num;
        char                    info[MAX_STRING];
        unsigned short          info_size;
};

typedef struct _sice_stun_attr_string                    attr_string;
struct _sice_stun_attr_string {
        char            val[MAX_STRING];
        unsigned short  val_size;
};

typedef struct _sice_stun_attr_unknown                    attr_unknown;
struct _sice_stun_attr_unknown {
        
        unsigned short          attr_type[MAX_UNKNOWN_ATTR];
        unsigned short          attr_num;
};
 


typedef enum _sice_stun_hmac_status                      E_HMAC_STATUS;
enum _sice_stun_hmac_status {
  
        UNKNOWN_HMAC            =       0       ,
        UNKNOWN_USERNAME_HMAC                   ,
        UNKNOWN_BAD_USERNAME                    ,
        OK_HMAC                                 ,
        FAILED_HMAC                             

};

typedef struct _sice_stun_message                stun_message;
struct _sice_stun_message {

    stun_message_header msg_header;
    attr_addr4*         source_addr;
    attr_addr4*         response_addr;
    attr_addr4*         mapped_addr;
    attr_addr4*         xor_mapped_addr;
    attr_addr4*         changed_addr;
    attr_addr4*         relay_addr;     // reflected from
    attr_string*        username_attr;
    attr_string*        password_attr;
    attr_string*        string_attr;
    attr_error*         err_attr;
    attr_unknown*       u_attr;
    attr_change_req*    change_req_attr;
    attr_integrity*     integrity_attr;
};
                 


/* proto's*/

int
sice_stun_encode_message (       stun_message*     msg, // dest
                                 char*                   buffer, //src
                                 unsigned int            buffer_len,
                                 attr_string*            pw );


sice_stun_message*
sice_parse_stun_message (        char*                  buffer,
                                 unsigned int           buffer_len );

attr_string*
sice_stun_form_username (        const attr_addr4*           source_attr );

#ifdef __cplusplus
}
#endif

#endif /* _SICE_STUN_H_ */
