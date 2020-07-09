
#include "ngx_config.h"
#include "ngx_core.h"

#include "ngx_filed.h"

ngx_message_codec_op_t ngx_binary_codecs[] = {

    {NGX_HEARTBEAT_REQ_MSG_ID,      ngx_binary_decode_heartbeat,           ngx_binary_encode_heartbeat},
    {NGX_HEARTBEAT_RSP_MSG_ID,      ngx_binary_decode_heartbeat,           ngx_binary_encode_heartbeat},
    {NGX_LOGIN_REQ_MSG_ID,          ngx_binary_decode_login_req,           ngx_binary_encode_login_req},
    {NGX_LOGIN_RSP_MSG_ID,          ngx_binary_decode_login_rsp,           ngx_binary_encode_login_rsp},
    {NGX_MOBILECODE_REQ_MSG_ID,     ngx_binary_decode_mobilecode_req,      ngx_binary_encode_mobilecode_req},
    {NGX_MOBILECODE_RSP_MSG_ID,     ngx_binary_decode_mobilecode_rsp,      ngx_binary_encode_mobilecode_rsp}

};


ngx_protocol_module_t ngx_binary_module = {

    NGX_BINARY_PTOTOCOL,
	sizeof(ngx_binary_codecs) / sizeof(ngx_message_codec_op_t),
	ngx_binary_codecs
};


void*  
ngx_binary_decode_heartbeat(ngx_char_t* buffer, ngx_uint_t len){

    ngx_heartbeat_msg_t* msg = (ngx_heartbeat_msg_t*)malloc(sizeof(ngx_heartbeat_msg_t));
	msg->uid = ngx_get_filed_int(buffer);

    return msg;
}


ngx_int_t 
ngx_binary_encode_heartbeat(ngx_char_t* buffer, void* message){

    ngx_set_filed_int(buffer, &((ngx_heartbeat_msg_t*)message)->uid );
	return sizeof(ngx_int_t);

}


void* 
ngx_binary_decode_login_req(ngx_char_t* buffer, ngx_uint_t len){

    ngx_login_req_msg_t* msg = (ngx_login_req_msg_t*)malloc(sizeof(ngx_login_req_msg_t));
	msg->uid = ngx_get_filed_int(buffer);
	msg->pwd_len = buffer[offsetof(ngx_login_req_msg_t, pwd_len)];
	memcpy(msg->pwd, buffer[offsetof(ngx_login_req_msg_t, pwd)], msg->pwd_len);

	return msg;
}


ngx_int_t 
ngx_binary_encode_login_req(ngx_char_t* buffer, void* message){

    ngx_login_req_msg_t* msg = (ngx_login_req_msg_t*)message;
	ngx_set_filed_int(buffer, msg->uid);
	buffer[offsetof(ngx_login_req_msg_t, pwd_len)] = msg->pwd_len;
	memcpy(buffer + offsetof(ngx_login_req_msg_t, pwd), msg->pwd, msg->pwd_len);
	return sizeof(ngx_login_req_msg_t);
}


void* 
ngx_binary_decode_login_rsp(ngx_char_t* buffer, ngx_uint_t len){

    ngx_login_rsp_msg_t* msg = (ngx_login_rsp_msg_t*)malloc(sizeof(ngx_login_rsp_msg_t));
	msg->uid = ngx_get_filed_int(buffer);
    msg->status = buffer[offsetof(ngx_login_rsp_msg_t, status)];
	
	return msg;
}

ngx_int_t 
ngx_binary_encode_login_rsp(ngx_char_t* buffer, void* message){

    ngx_login_rsp_msg_t* msg = (ngx_login_rsp_msg_t*)message;
	ngx_set_filed_int(buffer, msg->uid);
	buffer[offsetof(ngx_login_rsp_msg_t, status)] = msg->status;
	return sizeof(ngx_login_rsp_msg_t);
}


void* 
ngx_binary_decode_mobilecode_req(ngx_char_t* buffer, ngx_uint_t len){

    ngx_mobilecode_req_msg_t* msg = (ngx_mobilecode_req_msg_t*)malloc(sizeof(ngx_mobilecode_req_msg_t));
	msg->len = buffer[0];
	memcpy(msg->mobile, buffer + offsetof(ngx_mobilecode_req_msg_t, mobile), msg->len);
    return msg;
}


ngx_int_t 
ngx_binary_encode_mobilecode_req(ngx_char_t* buffer, void* message){

    ngx_mobilecode_req_msg_t* msg = (ngx_mobilecode_req_msg_t*)message;
    buffer[0] = msg->len;
	memcpy(buffer + offsetof(ngx_mobilecode_req_msg_t, mobile), msg->mobile, msg->len);
	return sizeof(ngx_mobilecode_req_msg_t);
}


void* 
ngx_binary_decode_mobilecode_rsp(ngx_char_t* buffer, ngx_uint_t len){

    ngx_mobilecode_rsp_msg_t* msg = (ngx_mobilecode_rsp_msg_t*)malloc(sizeof(ngx_mobilecode_rsp_msg_t));
    msg->len = buffer[0];
	memcpy(msg->mobile, buffer + offsetof(ngx_mobilecode_rsp_msg_t, mobile), msg->len);
	msg->clen = buffer + offsetof(ngx_mobilecode_rsp_msg_t, clen);
	memcpy(msg->code, buffer + offsetof(ngx_mobilecode_rsp_msg_t, code), msg->clen);
	return msg;
}


ngx_int_t 
ngx_binary_encode_mobilecode_rsp(ngx_char_t* buffer, void* message){

    ngx_mobilecode_rsp_msg_t* msg = (ngx_mobilecode_rsp_msg_t*)message;
	memcpy(buffer, &msg, sizeof(ngx_mobilecode_rsp_msg_t));
    return sizeof(ngx_mobilecode_rsp_msg_t);
}


