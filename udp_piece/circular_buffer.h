#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
/* 参考地址https://www.2cto.com/kf/201305/208947.html */

typedef struct circular_buffer
{
	int	beg_index_;
	int	end_index_;
	int	size_;
	int	capacity_;
	uint8_t *data_;
}circular_buffer_t;


circular_buffer_t *circular_buffer_init( int capacity );


void circular_buffer_deinit( circular_buffer_t *circular_buffer );


void circular_buffer_reset( circular_buffer_t *circular_buffer );


int circular_buffer_write( circular_buffer_t *circular_buffer, const uint8_t *data, int bytes );


int circular_buffer_read( circular_buffer_t *circular_buffer, uint8_t *data, int bytes );


int circular_buffer_get( circular_buffer_t *circular_buffer, int index, uint8_t *value );


void circular_buffer_pop_front( circular_buffer_t *circular_buffer, const int pop_size);


int circular_buffer_size( circular_buffer_t *circular_buffer );


int circular_buffer_capacity( circular_buffer_t *circular_buffer );

#ifdef __cplusplus
}
#endif /* end of __cplusplus */

#endif
