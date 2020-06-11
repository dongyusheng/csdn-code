
#ifndef _INFLATING_STREAM_H_
#define _INFLATING_STREAM_H_
#include <iostream>

#include <zlib.h>

class InflatingStream
{
public:
	enum StreamType
	{
		STREAM_ZLIB, /// Expect a zlib header, use Adler-32 checksum.
		STREAM_GZIP /// Expect a gzip header, use CRC-32 checksum.
	};
	/**
	 * @brief: 解压初始化, 如果初始化异常则抛出异常, 所以调用者需要捕获异常
	 * @param type 解压类型，目前只用来测试STREAM_GZIP
	 * @return: 无
	 */
	InflatingStream(StreamType type);

	/**
	 * @brief: 析构释放资源
	 */
	~InflatingStream();
		/// Destroys the InflatingStream.
	/**
	 * @brief: 解压数据
	 * @param inBuf 要解压的数据,如果传入NULL则说明要解压的数据已经传输完毕
	 * @param inLen 要解压数据的长度
	 * @param outStr 输出解压后的数据，如果size为0则说明数据还在缓冲区
	 * @return: 0 正常, -1 异常
	 */	
	int Inflate(const char* inBuf, const size_t inLen, std::string &outBuf);
private:
	enum 
	{
		STREAM_BUFFER_SIZE  = 1024,
		INFLATE_BUFFER_SIZE = 32768,
        ORIGN_NAME_BUFFER_SIZE = 128
	};
	
	char*    _buffer;
    char*   _orignNameBuffer; 
	z_stream _zstr;
    gz_header _gzipHead;
	bool     _eof;
};

#endif