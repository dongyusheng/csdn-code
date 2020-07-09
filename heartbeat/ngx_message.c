
#include "ngx_config.h"
#include "ngx_core.h"

#include "ngx_filed.h"


ngx_protocol_modules_t ngx_protocol_modules = {

    ngx_message_decode_head,
    ngx_message_encode_head,
    ngx_binary_module,
    
};


void* 
ngx_message_decode_head(ngx_char_t* buffer, ngx_uint_t len){

    ngx_message_head_t*  head = (ngx_message_head_t*)malloc(sizeof(ngx_message_head_t));

	head->magic[0] = buffer[0];
	head->magic[1] = buffer[1];
	head->version  = ngx_get_filed_short(buffer + offsetof(ngx_message_head_t, version));
	head->type     = ngx_get_filed_short(buffer + offsetof(ngx_message_head_t, type));
	head->len      = ngx_get_filed_short(buffer + offsetof(ngx_message_head_t, len));
	head->seq      = ngx_get_filed_int(buffer + offsetof(ngx_message_head_t, seq));
	head->id       = ngx_get_filed_short(buffer + offsetof(ngx_message_head_t, id));
	memcpy(head->reserve, buffer + offsetof(ngx_message_head_t, reserve));

	return head;
}


ngx_int_t 
ngx_message_encode_head(ngx_char_t* buffer, void* head){

    ngx_message_head_t*  h = (ngx_message_head_t*)head;
	buffer[0]  = h->magic[0];
	buffer[1]  = h->magic[1];
	ngx_set_filed_short(buffer + offsetof(ngx_message_head_t, version), h->version);
	ngx_set_filed_short(buffer + offsetof(ngx_message_head_t, type), h->type);
	ngx_set_filed_short(buffer + offsetof(ngx_message_head_t, len), h->len);
	ngx_set_filed_int(buffer + offsetof(ngx_message_head_t, seq), h->seq);
	ngx_set_filed_short(buffer + offsetof(ngx_message_head_t, id), h->id);
	buffer + offsetof(ngx_message_head_t, reserve)     = h->reserve[0];
	buffer + offsetof(ngx_message_head_t, reserve) + 1 = h->reserve[1];

	return sizeof(ngx_message_head_t);
}



