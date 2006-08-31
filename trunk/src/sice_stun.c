/* Copyright (C) 2006  Movial Oy
 * authors:     rami.erlin@movial.fi
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

/* stun specific part of ice library */

#include "../include/sice_types.h"
#include "sice_stun.h"
#include "sha1.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <net/if.h>


#include <glib.h>



attr_addr4* /*!    this should work now    */
sice_parse_stun_attribute_address (      int             header_len,
                                        //attr_addr4      attribute_address,
                                        char*           data) {

    if ( header_len != 8 ) return NULL;
    
    attr_addr4* attribute_address;
    attribute_address = ( attr_addr4* ) malloc( sizeof( attr_addr4 ) );
    memset( attribute_address, 0 , sizeof( attr_addr4 ) );
    attribute_address->pad = *data++;
    
    if ( !(FAMILY_IPv4 == (attribute_address->ip_family = *data++ )) ) {
            /*! IPv6 is not supported */
    }
    else {      
        unsigned short tmp16;   /* for port */
        unsigned int tmp32;     /* for address */
        /* parsing port */ 
        memcpy( &tmp16, data, sizeof( attribute_address->ip.port ));
        data += sizeof( attribute_address->ip.port );
        attribute_address->ip.port = ntohs( tmp16 );
        /* parsing address */
        memcpy( &tmp32, data, sizeof( attribute_address->ip.addr ));
        data += sizeof( attribute_address->ip.addr );
        attribute_address->ip.addr = ntohl( tmp32 );

    }
    return attribute_address;
}

sice_stun_message*
sice_parse_stun_message (        char*                  buffer,
                                unsigned int           buffer_len ) {

    sice_stun_message* msg = NULL;
    msg = (sice_stun_message* )malloc( sizeof( sice_stun_message ) );

    if ( msg == NULL ) 
        return msg;

    memset ( msg, 0 ,sizeof( sice_stun_message )); // set all to 0

   /* TODO:check that is more than msg header in buffer*/
    
    if ( buffer_len < msg->msg_header.message_length + sizeof( stun_message_header ))
        return NULL;

    /* take message header out from buffer */
    memcpy( &msg->msg_header, buffer, sizeof( stun_message_header ) );
    msg->msg_header.message_type = ntohs ( msg->msg_header.message_type );
    msg->msg_header.message_length = ntohs ( msg->msg_header.message_length );

    /* TODO: if msg_header + msg_lengt >buffer )  error !!!! */

    char* data = buffer + sizeof( message_header );

    int s = msg->msg_header.message_length;
    E_STUN_MSG_ATTR msg_attr_type;
   
    attribute_header*    attr_hdr = NULL;

    for (; s > 0 ; s -= sizeof( int )) {
        
        attr_hdr = ( attribute_header* )data; 
        
    /*  hop over lenght and type in attribute header */
        data += sizeof( int ); // attribute len is 32 bit's
        
        unsigned short msg_attr_len = ntohs( attr_hdr->attribute_len );
        msg_attr_type = ( E_STUN_MSG_ATTR ) ntohs( attr_hdr->attribute_type );
        
        /*TODO: check if attr is larger than mesg, if it is return null*/
       
       switch ( msg_attr_type ) {

            case    MAPPED_ADDRESS:
                  //  if ( ! msg->mapped_address )
                        msg->mapped_address = /* check this later return value is pointer */
                        sice_parse_stun_attribute_address ( 
                                    ntohs( attr_hdr->attribute_len ),   /*! */ 
                                    data );
                break;

            
            case    RESPONSE_ADDRESS:
                  //  if ( ! msg->destination_addr )                    
                        msg->destination_addr =
                        sice_parse_stun_attribute_address ( 
                                    ntohs( attr_hdr->attribute_len ), 
                                    data );
                break;

            case    CHANGE_REQUEST:
                  //  if ( ! msg->
                    msg->change_req_attr = (attr_change_req*)malloc( sizeof( attr_change_req ));
                    
                    if ( attr_hdr->attribute_len == sizeof( int ) ) {
                        memcpy( &(msg->change_req_attr->val) , data , sizeof( int ) );
                        msg->change_req_attr->val = ntohl( msg->change_req_attr->val );
                    }
                          
                break;
    
            case    USERNAME:
                     
                break;

            case    PASSWORD:
            
                break;

            case    MESSAGE_INTEGRITY:
                    //if ( ! msg ->
                    msg->integrity_attr = (attr_integrity*)malloc( sizeof ( attr_integrity ) );
                    if ( msg->integrity_attr != NULL ) {
                        memset( msg->integrity_attr, 0 , sizeof( attr_integrity ));
                        memcpy( msg->integrity_attr->sha1_hash, data, msg_attr_len );
                    }
                    
                           
                break;

            case    ERROR_CODE:
        //            if ( ! msg->err_attr )
        //                msg->err_attr =
        //                    sice_stun
                break;

            case    UNKNOWN_ATTRIBUTES:
                    
                    msg->u_attr = (attr_unknown*)malloc( sizeof( attr_unknown ));

                    if (           ( msg->u_attr != NULL ) 
                                && ( msg_attr_len < sizeof ( attr_unknown ) ) 
                                && ( msg_attr_len % 4 == 4 ) ) {
                                
                            msg->u_attr->attr_num = attr_hdr->attribute_len / 4;
                            
                            int i;
                            for ( i = 0; i < msg->u_attr->attr_num ; i++ ) {
                                memcpy( &(msg->u_attr->attr_type[i]), data, 2 );
                                data +=2;
                                msg->u_attr->attr_type[i] = ntohs( msg->u_attr->attr_type[i] );
                            }
                    }
                     
                break;

            case    REFLECTED_FROM: // relay ??
                  //  if ( ! msg->relay_addr )
                    msg->relay_addr =
                    sice_parse_stun_attribute_address ( 
                            ntohs( attr_hdr->attribute_len ), 
                            data );
                break;

            case    SOURCE_ADDRESS:       
                  //  if ( ! msg->source_addr )
                    msg->source_addr =
                    sice_parse_stun_attribute_address ( 
                            ntohs( attr_hdr->attribute_len ), 
                            data );
                break;
            default:
                break;
        }
        s--;
    }
    return msg;
}


static char*
sice_stun_encode (      char*           buffer,
                        const char*     data,
                        unsigned short  len ) {
    
    buffer = (char*)memcpy( buffer, data, len );
    
    return buffer + len;
}


static char*
sice_stun_encode16 (    char*                   buffer,
                        unsigned short          data ) {
        
    unsigned short *tmp = (unsigned short*) buffer;
    *tmp = htons ( (unsigned short) data );
    
    return buffer + 2; //sizeof ( data );
}

static char* 
sice_stun_encode32 (    char*           buffer,
                        unsigned int    data ) {
    
    unsigned int *tmp = (unsigned int*) buffer;
    *tmp = htonl ( (unsigned int) data );
    
    return buffer + 4; // sizeof ( data );
}
        

static char*
sice_stun_encode_attr_change_req (      char*                   p,
                                        const attr_change_req*  a ) {
    
    p = sice_stun_encode16 ( p, (unsigned short)CHANGE_REQUEST);
    p = sice_stun_encode16 ( p, (unsigned short) 4 );
    p = sice_stun_encode32 ( p, (unsigned int) a->val );
    return p;
}

static char*
sice_stun_encode_attr_error (           char*                   p,
                                        const attr_error*       a ) {

    p = sice_stun_encode16 ( p, (unsigned short)ERROR_CODE);
    p = sice_stun_encode16 ( p, (unsigned short) (6 + a->info_size ) );
    p = sice_stun_encode32 ( p, (unsigned int) a->pad);
    *p++ = a->err_class;
    *p++ = a->num;
    p = sice_stun_encode ( p , a->info, a->info_size );

    return p;
}

static char*
sice_stun_encode_attr_unknown (         char*                   p,
                                        const   attr_unknown*   a ) {

    int i = -1;

    p = sice_stun_encode16 ( p, (unsigned short)UNKNOWN_ATTRIBUTES);
    p = sice_stun_encode16 ( p, (unsigned short)(2 + a->attr_num + a->attr_num));
    
    while ( ++i != a->attr_num )// 0 - 7
        p = sice_stun_encode16 ( p, a->attr_type[ i ]);
    
    return p;
}

static char *
sice_stun_encode_attr_integrity (       char*                   p,
                                        const   attr_integrity* a ) {
 
    p = sice_stun_encode16 (p ,(unsigned short)MESSAGE_INTEGRITY),
    p = sice_stun_encode16 (p, 20); 
             
    return sice_stun_encode (p, a->sha1_hash, sizeof( a->sha1_hash ));
}


static char*
sice_stun_encode_attr_string (          char*                   p,
                                        unsigned short          type,
                                        const attr_string*      a ) {

    if (a->val_size % 4 != 0 ) { /* error*/ }
    else {

        p = sice_stun_encode16 ( p , type );
        p = sice_stun_encode16 ( p , a->val_size );

        return sice_stun_encode ( p, a->val ,a->val_size );
    }
  
    /* error occurred */
    return NULL;
}


static char*
sice_stun_encode_attr_address4 (       char*                   p,
                                       unsigned short          type,
                                       const attr_addr4*       a ) {
    const unsigned short dum = 8;
    
    p = sice_stun_encode16 (p,type );
    p = sice_stun_encode16 (p, dum );
    *p++ = a->pad;
    *p++ = FAMILY_IPv4;
    p = sice_stun_encode16 (p,a->ip.port);
    p = sice_stun_encode32 (p,a->ip.addr);
    
    return p;
}
int
sice_stun_encode_message (      const stun_message*     msg, // dest
                                char*                   buffer, //src
                                unsigned int            buffer_len,
                                attr_string*            pw ) {
    
    char*   p = buffer;
    if ( sizeof( message_header ) < buffer_len ) {
        char*   plen = NULL;
        p = sice_stun_encode16 ( p , msg->msg_header.message_type );
        plen = p;
        
        p = sice_stun_encode16( p , 0 );
        p = sice_stun_encode(   p,
                               (const char*) &msg->msg_header.message_id.oct,
                               sizeof( msg->msg_header.message_id ) );
        
        /* start to encode attributes */
        if ( msg->source_addr )         {
            
            p = sice_stun_encode_attr_address4(  p,
                                                SOURCE_ADDRESS,
                                                msg->source_addr );
                                                
        }       
        if ( msg->destination_addr )    {

            p = sice_stun_encode_attr_address4(  p,
                                                RESPONSE_ADDRESS,
                                                msg->destination_addr);
        }
        if ( msg->mapped_address )      {

            p = sice_stun_encode_attr_address4(  p,
                                                MAPPED_ADDRESS,
                                                msg->mapped_address );
        }
        if ( msg->xor_mapped_addr )     {
        
        }
        if ( msg->remote_addr )         {

        }
        if ( msg->username_attr )       {

        }
        if ( msg->password_attr )       {

        }
        if ( msg->relay_addr )          {

            p = sice_stun_encode_attr_address4(  p,
                                                REFLECTED_FROM,
                                                msg->relay_addr );
        }
        if ( msg->err_attr )            {
            
            p = sice_stun_encode_attr_error(     p,
                                                msg->err_attr );
        }
        if ( msg->string_attr )         {
            
          //  p = sice_stun_encode_attr_string(    p,
                                                
        }
        if ( msg->u_attr )              {
            
            p = sice_stun_encode_attr_unknown(   p,
                                                msg->u_attr );
        }
       if ( msg->change_req_attr )      {
            
            p = sice_stun_encode_attr_change_req(p,
                                                msg->change_req_attr );
       }
       if ( pw )                        {
           if ( pw->val_size > 0 ) {
                attr_integrity  integrity;
             //   hmac_sha1(  , , , , ); 
             //                
                p = sice_stun_encode_attr_integrity ( p , &integrity );                
           }
       }
       
        (void*)sice_stun_encode16(    plen, 
                    (unsigned short)(p-buffer-sizeof(message_header))); 
        

   }else { return -1; }

   return (int)(p - buffer);
}      


static void
sice_stun_to_hex(       const char*     buf, 
                        int             buf_size, 
                        char*           outp ) {
    
    static char map[] = "0123456789abcdef";
    const char* p = buf;
    char* x = outp;
    int i = 0;
    for ( i = 0; i < buf_size ; i++ ) {
        unsigned char tmp = *p++;
        int high = (tmp & 0xf0)>>4;
        int low = (tmp & 0xf);
        *x++ = map[high];
        *x++ = map[low];
    }
    *x = 0;
}


    



 /* _SICE_STUN_H_ */



