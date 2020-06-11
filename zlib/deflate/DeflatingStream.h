#ifndef _DEFLATING_STREAM_H_
#define _DEFLATING_STREAM_H_
#include <iostream>
#include <zlib.h>

enum StreamType
{
	STREAM_ZLIB = 0, /// Create a zlib header, use Adler-32 checksum.
	STREAM_GZIP		 /// Create a gzip header, use CRC-32 checksum.
};

class DeflatingStream
{
public:
	/**
	 * @brief: 压缩初始化, 如果初始化异常则抛出异常, 所以调用者需要捕获异常
	 * @param type 压缩类型，目前只用来测试STREAM_GZIP
	 * @param level 压缩级别 0~9, 级别越高压缩效率越高
	 * @param orignName 压缩后的文件名, 支持127个字节长度
	 * @return: 无
	 */
	DeflatingStream(const StreamType type, const int level, const std::string orignName = "");

	/**
	 * @brief: 析构释放资源
	 */
	~DeflatingStream();

	/**
	 * @brief: 压缩数据
	 * @param inBuf 要压缩的数据,如果传入NULL则说明要压缩的数据已经传输完毕
	 * @param inLen 要压缩数据的长度
	 * @param outStr 输出压缩后的数据，如果size为0则说明数据还在缓冲区
	 * @return: 0 正常, -1 异常
	 */
	int Deflate(char *inBuf, size_t inLen, std::string &outStr);

private:
	enum
	{
		STREAM_BUFFER_SIZE = 1024,
		DEFLATE_BUFFER_SIZE = 32768,
		ORIGN_NAME_BUFFER_SIZE = 128
	};

	char *_buffer;
	char *_orignNameBuffer;
	z_stream _zstr;
	gz_header _gzipHead;
	bool _eof;
};

#endif