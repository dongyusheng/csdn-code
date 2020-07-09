#include <string.h>
#include <stdlib.h>

#include "circular_buffer.h"


#define MIN( a, b ) (a) < (b) ? (a) : (b)

circular_buffer_t *circular_buffer_init( int capacity )
{
	circular_buffer_t *circular_buffer = (circular_buffer_t *) malloc( sizeof(circular_buffer_t) );
	if ( !circular_buffer )
		return(NULL);
	memset( circular_buffer, 0, sizeof(circular_buffer_t) );
	circular_buffer->capacity_ = capacity;

	circular_buffer->data_ = (char *) malloc( capacity );
	if ( !circular_buffer->data_ )
	{
		free( circular_buffer );
		circular_buffer = NULL;
	}
	return(circular_buffer);
}


void circular_buffer_deinit( circular_buffer_t *circular_buffer )
{
	if ( !circular_buffer )
		return;

	if ( circular_buffer->data_ )
		free( circular_buffer->data_ );
	circular_buffer->data_ = NULL;

	free( circular_buffer );
}


void circular_buffer_reset( circular_buffer_t *circular_buffer )
{
	if ( !circular_buffer )
		return;

	circular_buffer->beg_index_	= 0;
	circular_buffer->end_index_	= 0;
	circular_buffer->size_		= 0;
}


int circular_buffer_write( circular_buffer_t *circular_buffer, const uint8_t *data, int bytes )
{
	if ( bytes == 0 )
		return(0);

	int	capacity	= circular_buffer->capacity_;
	int	bytes_to_write	= MIN( bytes, capacity - circular_buffer->size_ );

	/* Write in a single step */
	if ( bytes_to_write <= capacity - circular_buffer->end_index_ )
	{
		memcpy( circular_buffer->data_ + circular_buffer->end_index_, data, bytes_to_write );
		circular_buffer->end_index_ += bytes_to_write;
		if ( circular_buffer->end_index_ == capacity )
			circular_buffer->end_index_ = 0;
	}
	/* Write in two steps */
	else
	{
		int size_1 = capacity - circular_buffer->end_index_;
		memcpy( circular_buffer->data_ + circular_buffer->end_index_, data, size_1 );
		int size_2 = bytes_to_write - size_1;
		memcpy( circular_buffer->data_, data + size_1, size_2 );
		circular_buffer->end_index_ = size_2;
	}

	circular_buffer->size_ += bytes_to_write;
	return(bytes_to_write);
}


int circular_buffer_read( circular_buffer_t *circular_buffer, uint8_t *data, int bytes )
{
	if ( bytes == 0 )
		return(0);

	int	capacity	= circular_buffer->capacity_;
	int	bytes_to_read	= MIN( bytes, circular_buffer->size_ );

	/* Read in a single step */
	if ( bytes_to_read <= capacity - circular_buffer->beg_index_ )
	{
		memcpy( data, circular_buffer->data_ + circular_buffer->beg_index_, bytes_to_read );
		circular_buffer->beg_index_ += bytes_to_read;
		if ( circular_buffer->beg_index_ == capacity )
			circular_buffer->beg_index_ = 0;
	}
	/* Read in two steps */
	else
	{
		int size_1 = capacity - circular_buffer->beg_index_;
		memcpy( data, circular_buffer->data_ + circular_buffer->beg_index_, size_1 );
		int size_2 = bytes_to_read - size_1;
		memcpy( data + size_1, circular_buffer->data_, size_2 );
		circular_buffer->beg_index_ = size_2;
	}

	circular_buffer->size_ -= bytes_to_read;
	return(bytes_to_read);
}


/* 通过索引取值 */
int circular_buffer_get( circular_buffer_t *circular_buffer, int index, uint8_t *value )
{
	if ( index >= circular_buffer->size_ )
		return(-1);

	int	capacity	= circular_buffer->capacity_;
	int	temp_index	= (index + circular_buffer->beg_index_) % capacity;

	*value = *(circular_buffer->data_ + temp_index);
	
	return(0);
}


/* 最前n个元素出队列 */
void circular_buffer_pop_front( circular_buffer_t *circular_buffer, const int pop_size)
{
	int	capacity	= circular_buffer->capacity_;
	int	pop_bytes	= MIN( pop_size, circular_buffer->size_ );

	/* pop in a single step */
	if ( pop_bytes <= capacity - circular_buffer->beg_index_ )
	{
		circular_buffer->beg_index_ += pop_bytes;
		if ( circular_buffer->beg_index_ == capacity )
			circular_buffer->beg_index_ = 0;
	}
	/* pop in two steps */
	else
	{
		int	size_1	= capacity - circular_buffer->beg_index_;
		int	size_2	= pop_bytes - size_1;
		circular_buffer->beg_index_ = size_2;
	}

	circular_buffer->size_ -= pop_bytes;
}


int circular_buffer_size( circular_buffer_t *circular_buffer )
{
	return(circular_buffer->size_);
}


int circular_buffer_capacity( circular_buffer_t *circular_buffer )
{
	return(circular_buffer->capacity_);
}


