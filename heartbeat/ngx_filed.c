
#include "ngx_config.h"
#include "ngx_core.h"

#include "ngx_filed.h"
#include <string.h>


ngx_int_t
ngx_get_filed_int(ngx_char_t* buffer){

    ngx_int_t filed = 0;
	memcpy((void*)&filed, buffer, sizeof(ngx_int_t));
	filed = htonl(filed);
    return filed;
}

void 
ngx_set_filed_int(ngx_char_t* buffer, ngx_int_t filed){

    filed = ntohl(filed);
	memcpy(buffer, (void*)&filed, sizeof(ngx_int_t));
    return;
}

ngx_short_t
ngx_get_filed_short(ngx_char_t* buffer){

    ngx_short_t filed = 0;
	memcpy((void*)&filed, buffer, sizeof(ngx_short_t));
	filed = htons(filed);
    return filed;

}

void 
ngx_set_filed_short(ngx_char_t* buffer, ngx_short_t filed){

   filed = ntohs(filed);
   memcpy(buffer, (void*)&filed, sizeof(ngx_short_t));
   return;
}


ngx_long_t
ngx_get_filed_long(ngx_char_t* buffer){

    ngx_int_t num = 42;
    ngx_long_t filed;
	
	memcpy((void*)&filed, buffer, sizeof(ngx_long_t));
    if(*(ngx_char_t *)&num == 42){  //Little Endian
        return ( (ngx_long_t)htonl((ngx_int_t)(filed>>32&0xffffffff)) )|( ((ngx_long_t)htonl((ngx_int_t)(filed&0xffffffff)))<<32 );
    }
    else{  // Big Endian
        return filed; 
    }

    return filed;
}

void 
ngx_set_filed_long(ngx_char_t* buffer, ngx_long_t filed){

    ngx_int_t num = 42;
    if(*(ngx_char_t *)&num == 42){  //Little Endian
        filed = (((unsigned long long )htonl((int)((filed << 32) >> 32))) << 32) | (unsigned int)htonl((int)(filed >> 32));
    }
    else{  // Big Endian
        
    }

	memcpy(buffer, &filed, sizeof(filed));

    return ;
}



