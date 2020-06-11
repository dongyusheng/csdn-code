#include "DeflatingStream.h"
#include <cstring>
DeflatingStream::DeflatingStream(const StreamType type, const int level, const std::string orignName) : _eof(false)
{
    _zstr.zalloc = Z_NULL;
    _zstr.zfree = Z_NULL;
    _zstr.opaque = Z_NULL;
    _zstr.next_in = 0;
    _zstr.avail_in = 0;
    _zstr.next_out = 0;
    _zstr.avail_out = 0;

    _buffer = new char[DEFLATE_BUFFER_SIZE];
    _orignNameBuffer = new char[ORIGN_NAME_BUFFER_SIZE];
    int rc = deflateInit2(&_zstr,
                          level, 
                          Z_DEFLATED,
                          MAX_WBITS + (type == STREAM_GZIP ? 16 : 0), 
                          MAX_MEM_LEVEL, 
                          Z_DEFAULT_STRATEGY);
    if (rc != Z_OK)
    {
        delete [] _buffer;
        delete [] _orignNameBuffer;
        _buffer = NULL;
        _orignNameBuffer = NULL;
        printf("DeflatingStream failed:%s\n", zError(rc));
        throw "deflateInit2 failed";
        return;
    }

    if (STREAM_GZIP == type)
    {
        memset(&_gzipHead, sizeof(gz_header), 0);
        strcpy(_orignNameBuffer, orignName.c_str());
        _gzipHead.name = (Bytef *)_orignNameBuffer ;
        _gzipHead.name_max = strlen(orignName.c_str()) + 1; // 目前只显示16字节的文件名
        printf("_gzipHead.name:%s, size:%d\n", _gzipHead.name, _gzipHead.name_max);
        int rc = deflateSetHeader(&_zstr, &_gzipHead);
        printf("deflateSetHeader = %d\n", rc);
    }
}

DeflatingStream::~DeflatingStream()
{
    if (_buffer)
        delete[] _buffer;
    if(_orignNameBuffer)
        delete [] _orignNameBuffer;
    deflateEnd(&_zstr);
}

int DeflatingStream::Deflate(char *inBuf, size_t inLen, std::string &outStr)
{
    if (!inBuf || inLen == 0)
    {
        _eof = true;
    }
    _zstr.next_in = (unsigned char *)inBuf;    //输入数据
    _zstr.avail_in = static_cast<unsigned>(inLen); // 输入多少数据
    _zstr.next_out = (unsigned char *)_buffer; // 输出空间
    _zstr.avail_out = DEFLATE_BUFFER_SIZE;  // 输出空间的大小

    if (!_eof)
    {
        for (;;)
        {
            int rc = deflate(&_zstr, Z_NO_FLUSH);
            printf("%s(%d) ret:%d, avail_in:%u, avail_out:%u\n",
                   __FUNCTION__, __LINE__, rc, _zstr.avail_in, _zstr.avail_out);
            if (rc != Z_OK)
            {
                perror("deflate ");
                return -1;
            }
            if (_zstr.avail_out == 0) // 没有输出空间了
            {
                outStr.append(_buffer, _buffer + DEFLATE_BUFFER_SIZE);
                _zstr.next_out = (unsigned char *)_buffer;
                _zstr.avail_out = DEFLATE_BUFFER_SIZE;
            } 
            else if(_zstr.avail_in == 0) // 输入以完成
            {
                outStr.append(_buffer, DEFLATE_BUFFER_SIZE - _zstr.avail_out);
                _zstr.next_out = (unsigned char *)_buffer;
                _zstr.avail_out = DEFLATE_BUFFER_SIZE;
                break;
            }
        }
    }
    else // 不会再有数据进来
    {
        if (_zstr.next_out)
        {
            int rc = deflate(&_zstr, Z_FINISH);
            printf("%s(%d) ret:%d, avail_in:%u, avail_out:%u\n",
                   __FUNCTION__, __LINE__, rc, _zstr.avail_in, _zstr.avail_out);
            if (rc != Z_OK && rc != Z_STREAM_END)
            {
                perror("deflate ");
                return -1;
            }
            outStr.append(_buffer, DEFLATE_BUFFER_SIZE - _zstr.avail_out);
            _zstr.next_out = (unsigned char *)_buffer;
            _zstr.avail_out = DEFLATE_BUFFER_SIZE;
            while (rc != Z_STREAM_END) // 设置Z_FINISH的时候返回值为 Z_STREAM_END
            {
                rc = deflate(&_zstr, Z_FINISH);
                printf("%s(%d) ret:%d, avail_in:%u, avail_out:%u\n",
                       __FUNCTION__, __LINE__, rc, _zstr.avail_in, _zstr.avail_out);
                if (rc != Z_OK && rc != Z_STREAM_END)
                {
                    perror("deflate ");
                    return -1;
                }
                outStr.append(_buffer, DEFLATE_BUFFER_SIZE - _zstr.avail_out);
                _zstr.next_out = (unsigned char *)_buffer;
                _zstr.avail_out = DEFLATE_BUFFER_SIZE;
            }
        }
    }

    return 0;
}