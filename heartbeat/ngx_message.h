
#ifndef _NGX_MESSAGE_H_INCLUDED_
#define _NGX_MESSAGE_H_INCLUDED_

typedef enum {

    NGX_BINARY_PTOTOCOL   = 0,
	NGX_JSON_PROTOCOL     = 1,
	NGX_PROTOBUF_PROTOCOL = 2,
	NGX_FLATBUF_PROTOCOL  = 3,

	NGX_UNKNOW_PROTOCOL   = 0xFF

}ngx_protocol_type_t;

typedef enum{

    NGX_HEARTBEAT_REQ_MSG_ID    = 0,
	NGX_HEARTBEAT_RSP_MSG_ID    = 1,
	NGX_LOGIN_REQ_MSG_ID        = 2,
	NGX_LOGIN_RSP_MSG_ID        = 3,
	NGX_MOBILECODE_REQ_MSG_ID   = 4,
	NGX_MOBILECODE_RSP_MSG_ID   = 5

}ngx_message_id_t;

typedef struct {

    ngx_char_t   magic[2];    // magic number
    ngx_short_t  version;     // protocol version
    ngx_short_t  type;        // protocol type: json, xml, binary, protobuf, flatbuffer and so on
    ngx_short_t  len;         // body length
    ngx_uint_t   seq;         // message number
    ngx_short_t  id;          // message id, HEARTBEAT, LOGIN ...
    ngx_char_t   reserve[2];  // reserve

}ngx_message_head_t;

typedef struct {

    ngx_message_head_t* head;
	void*               body;

}ngx_message_t;



typedef void* (* ngx_message_decode_op_t)(ngx_char_t* buffer, ngx_uint_t len);
typedef ngx_int_t (* ngx_message_encode_op_t)(ngx_char_t* buffer, void* message);

void* 
ngx_message_decode_head(ngx_char_t* buffer, ngx_uint_t len);

ngx_int_t 
ngx_message_encode_head(ngx_char_t* buffer, void* message);


typedef struct {

    ngx_short_t id;
	ngx_message_decode_op_t decode;
	ngx_message_encode_op_t encode;

}ngx_message_codec_op_t;

typedef struct{

    ngx_short_t               type;
	ngx_int_t                 size;
	ngx_message_codec_op_t*   codecs;

}ngx_protocol_module_t;

typedef struct {

    ngx_message_decode_op_t  hdecode;   // head decode op
    ngx_message_encode_op_t  hencode;   // head encode op
    ngx_protocol_module_t    module[0]; // protocol module  

}ngx_protocol_modules_t;


extern ngx_protocol_modules_t ngx_protocol_modules;



typedef struct {

    ngx_uint_t   uid;

}ngx_heartbeat_msg_t;


typedef struct {

    ngx_uint_t   uid;
	ngx_char_t   pwd_len;
	ngx_char_t   pwd[31];

}ngx_login_req_msg_t;

typedef struct {

    ngx_uint_t   uid;
	ngx_char_t   status; // success or failed
	ngx_char_t   reserve[3];  // reserved filed

}ngx_login_rsp_msg_t;

typedef struct {

    ngx_char_t len;
	ngx_char_t mobile[15];

}ngx_mobilecode_req_msg_t;

typedef struct {

    ngx_char_t len;
	ngx_char_t mobile[15];
	ngx_char_t clen;
	ngx_char_t code[11];

}ngx_mobilecode_rsp_msg_t;



#endif


