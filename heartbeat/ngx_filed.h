
#ifndef _NGX_TIMES_H_INCLUDED_
#define _NGX_TIMES_H_INCLUDED_

#include "ngx_config.h"
#include "ngx_core.h"

ngx_int_t
ngx_get_filed_int(ngx_char_t* buffer);

void 
ngx_set_filed_int(ngx_char_t* buffer, ngx_int_t filed);


ngx_short_t
ngx_get_filed_short(ngx_char_t* buffer);

void 
ngx_set_filed_short(ngx_char_t* buffer, ngx_short_t filed);

ngx_short_t
ngx_get_filed_short(ngx_char_t* buffer);

void 
ngx_set_filed_short(ngx_char_t* buffer, ngx_short_t filed);

ngx_long_t
ngx_get_filed_short(ngx_char_t* buffer);

void 
ngx_set_filed_short(ngx_char_t* buffer, ngx_long_t filed);



#endif

