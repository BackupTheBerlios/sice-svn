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

/* stun specific part of ice library */

#include "../include/sice_types.h"
#include "sice_stun.h"
#include "sha1.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>


#include <glib.h>



attr_addr4*
sice_parse_stun_attribute_address (     unsigned int    header_len,
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
    
    if ( buffer_len < (msg->msg_header.message_length + sizeof( stun_message_header )))
        return NULL;

    /* take message header out from buffer */
    memcpy( &msg->msg_header, buffer, sizeof( stun_message_header ) );
    msg->msg_header.message_type = ntohs ( msg->msg_header.message_type );
    msg->msg_header.message_length = ntohs ( msg->msg_header.message_length );



    char* data = buffer + sizeof( message_header );

    int s = msg->msg_header.message_length;
    E_STUN_MSG_ATTR msg_attr_type;
   
    attribute_header*    attr_hdr = NULL;

    while( s > 0 ) {
        
        attr_hdr = ( attribute_header* )data; 
        
    /*  hop over lenght and type in attribute header */
        data += sizeof( int ); 
        s    -= sizeof( int );
        
        unsigned short msg_attr_len = (unsigned short)ntohs( attr_hdr->attribute_len );
        msg_attr_type = (unsigned short)ntohs( attr_hdr->attribute_type );
        
        /*TODO: check if attr is larger than msg, if it is return null*/
       
       switch ( msg_attr_type ) {

            case    MAPPED_ADDRESS:
                        msg->mapped_addr = 
                        sice_parse_stun_attribute_address ( 
                                    ntohs( attr_hdr->attribute_len ),   /*! */ 
                                    data );
                break;

            
            case    RESPONSE_ADDRESS:
                        msg->response_addr =
                        sice_parse_stun_attribute_address ( 
                                    ntohs( attr_hdr->attribute_len ), 
                                    data );
                break;

            case    CHANGE_REQUEST:
                    msg->change_req_attr = (attr_change_req*)malloc( sizeof( attr_change_req ));
                    
                    if ( attr_hdr->attribute_len == sizeof( int ) ) {
                        memcpy( &(msg->change_req_attr->val) , data , sizeof( int ) );
                        msg->change_req_attr->val = ntohl( msg->change_req_attr->val );
                    }
                          
                break;
    
            case    USERNAME:
                     printf("\nun_len: %d",msg_attr_len);
                    if (( msg_attr_len < MAX_STRING ) && 
                        ( msg_attr_len % 4 == 0 )) {
                        msg->username_attr = (attr_string*)malloc( sizeof(attr_string) );
                        memset( msg->username_attr, 0, sizeof(attr_string) );
                        msg->username_attr->val_size = msg_attr_len;
                        memcpy( msg->username_attr->val, data, msg_attr_len );
                        msg->username_attr->val[msg_attr_len] = 0;
                    }else{ /*error*/ }
                break;

            case    PASSWORD:
                         printf("\npw_len:%u", msg_attr_len );
                    if (( msg_attr_len < MAX_STRING ) && 
                        ( msg_attr_len % 4 == 0 )) {
                        msg->password_attr = (attr_string*)malloc( sizeof(attr_string) );
                        memset( msg->password_attr, 0, sizeof(attr_string) );
                        msg->password_attr->val_size = msg_attr_len;
                        memcpy( msg->password_attr->val, data, msg_attr_len );
                        msg->password_attr->val[msg_attr_len] = 0;
                    }else { /*error*/ }

                break;

            case    MESSAGE_INTEGRITY:
                    
                    msg->integrity_attr = (attr_integrity*)malloc( sizeof(attr_integrity) );
                    if ( msg->integrity_attr != NULL ) {
                        memset( msg->integrity_attr, 0 , sizeof( attr_integrity ));
                        memcpy( msg->integrity_attr->sha1_hash, data, msg_attr_len );
                    }
                   // s = 0; /* message parsed */
                break;

            case    ERROR_CODE:
                    
                    if ( msg_attr_len < sizeof(msg->err_attr) ) {
                        msg->err_attr = (attr_error*)malloc( sizeof(attr_error) );
                        memset( msg->err_attr, 0 , sizeof(attr_error) );
                        msg->err_attr->pad = *((unsigned short*)data);
                        msg->err_attr->pad = ntohs( msg->err_attr->pad );
                        data += 2;
                        msg->err_attr->err_class = *data++;
                        msg->err_attr->num = *data++;
                        msg->err_attr->info_size = msg_attr_len - 4;
                        memcpy( msg->err_attr->info, data, msg->err_attr->info_size );
                        msg->err_attr->info[msg->err_attr->info_size] = 0;
                    } else {/* error */}

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
                    
                    msg->relay_addr =
                    sice_parse_stun_attribute_address ( 
                            ntohs( attr_hdr->attribute_len ), 
                            data );
                break;

            case    SOURCE_ADDRESS:       
                    
                    msg->source_addr =
                    sice_parse_stun_attribute_address ( 
                            ntohs( attr_hdr->attribute_len ), 
                            data );
                break;

            case     CHANGED_ADDRESS:
                
                    msg->changed_addr =
                    sice_parse_stun_attribute_address (
                            ntohs( attr_hdr->attribute_len ),
                            data );
                break;

            default:
                
                break;
        }
        
        data += msg_attr_len;
        s    -= msg_attr_len;
    }

    return msg;
}


static char*
sice_stun_encode (      char*           buffer,
                        const char*     data,
                        unsigned short  len ) {
    
    memcpy( buffer, data, len );
    return buffer + len;
}


static char*
sice_stun_encode16 (    char*                   buffer,
                        unsigned short          data ) {
        
    unsigned short *tmp = (unsigned short*) buffer;
    *tmp = htons ( data );
    
    return buffer + 2; //sizeof ( data );
}

static char* 
sice_stun_encode32 (    char*           buffer,
                        unsigned int    data ) {
    
    unsigned int *tmp = (unsigned int*) buffer;
    *tmp = htonl ( data );
    
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
    p = sice_stun_encode ( p ,(const char*)a->info, (unsigned short)a->info_size );

    
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
             
    return sice_stun_encode (p, (const char*)a->sha1_hash, sizeof( a->sha1_hash ));
}


static char*
sice_stun_encode_attr_string (          char*                   p,
                                        unsigned short          type,
                                        const attr_string*      a ) {

    if (a->val_size % 4 != 0 ) { printf("\nwrong string length !!\n");/* error*/ }
    else {

        p = sice_stun_encode16 ( p , type );
        p = sice_stun_encode16 ( p , a->val_size );
        return sice_stun_encode ( p, a->val ,a->val_size );
    }
  
    /* error occurred, do something for this, we probably dont want SIGSEV if error*/
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
sice_stun_encode_message (      stun_message*     msg, // dest
                                char*                   buffer, //src
                                unsigned int            buffer_len,
                                attr_string*      pw ) {
    
    char*   p = buffer;
    if ( sizeof( message_header ) < buffer_len ) {
        char*   plen = NULL;
        p = sice_stun_encode16 ( p , msg->msg_header.message_type );
        plen = p;
        
        p = sice_stun_encode16( p , 0 );
        p = sice_stun_encode(   p,
                               (const char*) &(msg->msg_header.message_id.oct),
                               sizeof( msg->msg_header.message_id ) );
        
        /* start to encode attributes */
        if ( msg->mapped_addr )         {
            
            p = sice_stun_encode_attr_address4( p,
                                                MAPPED_ADDRESS,
                                                msg->mapped_addr );
            free ( msg->mapped_addr );                                        
        }       
        if ( msg->response_addr )    {

            p = sice_stun_encode_attr_address4( p,
                                                RESPONSE_ADDRESS,
                                                msg->response_addr);
            free ( msg->response_addr );
        }
        if ( msg->change_req_attr )      {
            
            p = sice_stun_encode_attr_change_req(       p,
                                                        msg->change_req_attr );
            free ( msg->change_req_attr );
        }

        if ( msg->source_addr )      {

            p = sice_stun_encode_attr_address4(         p,
                                                        SOURCE_ADDRESS,
                                                        msg->source_addr );
            free ( msg->source_addr );
        }
        if ( msg->changed_addr )         {

             p = sice_stun_encode_attr_address4(        p,
                                                        CHANGED_ADDRESS,
                                                        msg->changed_addr );
            free ( msg->changed_addr );
        }
        if ( msg->username_attr )       {
            
            p = sice_stun_encode_attr_string(   p,
                                                (unsigned short)USERNAME,
                                                msg->username_attr );
            free ( msg->username_attr );
        }
        if ( msg->password_attr )       {
 
            p = sice_stun_encode_attr_string(   p,
                                                (unsigned short)PASSWORD,
                                                msg->password_attr );
            free ( msg->password_attr );
        }
        if ( msg->relay_addr )          {

            p = sice_stun_encode_attr_address4( p,
                                                REFLECTED_FROM,
                                                msg->relay_addr );
            free ( msg->relay_addr );
        }
        if ( msg->err_attr )            {
            
            p = sice_stun_encode_attr_error(    p,
                                                msg->err_attr );
            free ( msg->err_attr );
        }
        if ( msg->u_attr )              {
            
            p = sice_stun_encode_attr_unknown(  p,
                                                msg->u_attr );
            free (  msg->u_attr );
        }
        if ( pw )                        {
            if ( pw->val_size > 0 ) {
                
                hmac_sha1(      pw->val, pw->val_size, buffer, 
                                (int)(p-buffer), msg->integrity_attr->sha1_hash ); 
                             
                p = sice_stun_encode_attr_integrity ( p , msg->integrity_attr );                
                free ( msg->integrity_attr );
            }
       }
       
        (void*)sice_stun_encode16(    plen, 
                    (unsigned short)(p-buffer-sizeof(message_header))); 
        

   } else { return -1; }

   return (int)(p - buffer);
}      


static void
sice_stun_hexer(        const char*     src, 
                        int             buf_size, 
                        char*           dest ) {
    
    static char map[] = "0123456789abcdef";
    const char* p = src;
    char* x = dest;
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

unsigned int 
sice_stun_rand_32b() {
   return (unsigned int)lrand48();
}
void
sice_stun_stamp_secs( unsigned long int* time ) {
    struct timeval now;
    gettimeofday( &now, NULL );
    *time = ( now.tv_sec - (now.tv_sec % 20 * 60 ));
}
    
attr_string*
sice_stun_form_username(        const attr_addr4*   source_attr ) {
    
    unsigned long time = 0;
    sice_stun_stamp_secs( &time );
    unsigned int ltime =(unsigned int)( time & 0xFFFFFFFF); 
    attr_string* username = NULL;
    char buf[1024];
    char hmac[20];
    char key[] = "Gustom";
    int  buflen = 0;
    char hexHmac[41];
    
    sprintf (   buf, "%08x%08x%08x",
                (unsigned int)source_attr->ip.addr,
                (unsigned int)sice_stun_rand_32b(),
                (unsigned int) ltime );

    if( (strlen(buf) < 1024) && ((strlen(buf) +41) < MAX_STRING) ) {
    
        hmac_sha1(      key, 
                        strlen(key), 
                        buf, 
                        strlen( buf ), hmac); 
        
        sice_stun_hexer( hmac, 20, hexHmac );
        strcat( buf, hexHmac );
        buflen = strlen( buf );
       
        printf("%d\n",buflen);
        if ( ((buflen +1) < MAX_STRING) && (buflen % 4 == 0) ) {
            username = (attr_string*)malloc( sizeof(attr_string) );
            memset(username, 0, sizeof(attr_string));
            username->val_size = buflen;
            memcpy( username->val, buf, buflen );
            username->val[buflen] = 0;
        }   
    }
    /* if any failures, return null*/
    return username;
}
    
attr_string*
sice_stun_form_password(    const attr_string*      username ) {
    char key[] = "Gustom";
    char hmac[20];
     
    hmac_sha1(  key, strlen( key ), 
                username->val, username->val_size, hmac ); 
        
    attr_string* password = (attr_string*)malloc( sizeof(attr_string) );
    memset( password, 0, sizeof(attr_string) );

    sice_stun_hexer( hmac, 20, password->val );
    
    password->val_size = strlen( password->val );
    password->val[ password->val_size ] = 0;

    return password;
}

 /* _SICE_STUN_H_ */



