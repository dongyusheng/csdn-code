
#ifndef _NGX_BINARY_PROTOCOL_H_INCLUDED_
#define _NGX_BINARY_PROTOCOL_H_INCLUDED_

#include "ngx_config.h"
#include "ngx_core.h"


extern ngx_protocol_module_t ngx_binary_module;


void*  
ngx_binary_decode_heartbeat(ngx_char_t* buffer, ngx_uint_t len);

ngx_int_t 
ngx_binary_encode_heartbeat(ngx_char_t* buffer, void* message);

void* 
ngx_binary_decode_login_req(ngx_char_t* buffer, ngx_uint_t len);

ngx_int_t 
ngx_binary_encode_login_req(ngx_char_t* buffer, void* message);

void* 
ngx_binary_decode_login_rsp(ngx_char_t* buffer, ngx_uint_t len);

ngx_int_t 
ngx_binary_encode_login_rsp(ngx_char_t* buffer, void* message);

void* 
ngx_binary_decode_mobilecode_req(ngx_char_t* buffer, ngx_uint_t len);

ngx_int_t 
ngx_binary_encode_mobilecode_req(ngx_char_t* buffer, void* message);

void* 
ngx_binary_decode_mobilecode_rsp(ngx_char_t* buffer, ngx_uint_t len);

ngx_int_t 
ngx_binary_encode_mobilecode_rsp(ngx_char_t* buffer, void* message);


#endif


