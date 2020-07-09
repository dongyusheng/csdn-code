
#include <sys/time.h>

#include "ngx_config.h"
#include "ngx_core.h"

volatile ngx_msec_t      ngx_current_msec;


void 
ngx_time_update(void){

    ngx_uint_t       sec;
    ngx_uint_t       msec;
    struct timeval   tv;
	
	gettimeofday(&tv, NULL);
	sec  = tv.tv_sec;
	msec = tv.tv_usec / 1000;
    
    ngx_current_msec = sec * 1000 + msec;
}



