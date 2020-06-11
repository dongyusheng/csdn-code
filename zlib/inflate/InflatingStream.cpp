
#include <cstring>
#include "InflatingStream.h"

InflatingStream::InflatingStream(StreamType type) : 
    _eof(false)
{
    _zstr.zalloc = Z_NULL;
    _zstr.zfree = Z_NULL;
    _zstr.opaque = Z_NULL;
    _zstr.next_in = 0;
    _zstr.avail_in = 0;
    _zstr.next_out = 0;
    _zstr.avail_out = 0;

    _buffer = new char[INFLATE_BUFFER_SIZE];
    _orignNameBuffer = new char[ORIGN_NAME_BUFFER_SIZE];
    int rc = inflateInit2(&_zstr, 15 + (type == STREAM_GZIP ? 16 : 0));
    if (rc != Z_OK)
    {
        delete [] _buffer;
        delete [] _orignNameBuffer;
         _buffer = NULL;
        _orignNameBuffer = NULL;
        printf("InflatingStream failed:%s\n", zError(rc));
        throw "inflateInit2 failed";
    }

    if (STREAM_GZIP == type)
    {
        memset(&_gzipHead, sizeof(gz_header), 0);
        _gzipHead.extra = (Bytef *)_orignNameBuffer;
        _gzipHead.extra_max = ORIGN_NAME_BUFFER_SIZE;
        _gzipHead.name = (Bytef *)_orignNameBuffer;
        _gzipHead.name_max = ORIGN_NAME_BUFFER_SIZE;
        _gzipHead.comment = (Bytef *)_orignNameBuffer;
        _gzipHead.comm_max = ORIGN_NAME_BUFFER_SIZE;
        int rc = inflateGetHeader(&_zstr, &_gzipHead);
        printf("inflateGetHeader = %d\n", rc);
    }
}

InflatingStream::~InflatingStream()
{
    if (_buffer)
        delete[] _buffer;
    if (_orignNameBuffer)  
        delete [] _orignNameBuffer;  
    inflateEnd(&_zstr);
}

int InflatingStream::Inflate(const char *inBuf, const size_t inLen, std::string &outBuf)
{
    if (!inBuf || inLen == 0)
    {
        _eof = true;
    }

    _zstr.next_in = (unsigned char *)inBuf;
    _zstr.avail_in = static_cast<unsigned>(inLen);
    _zstr.next_out = (unsigned char *)_buffer;
    _zstr.avail_out = INFLATE_BUFFER_SIZE;
    if (!_eof)
    {
        for (;;)
        {
            int rc = inflate(&_zstr, Z_NO_FLUSH);
            if(_gzipHead.done)
            {
                printf("inflate name:%s, extra:%s\n", _gzipHead.name, _gzipHead.extra);
            }
            if (rc == Z_STREAM_END)
            {
                outBuf.append(_buffer, _buffer + (INFLATE_BUFFER_SIZE - _zstr.avail_out));
                break;
            }
            if (rc != Z_OK)
            {
                perror("inflate error: ");
                return -1;
            }
            if (_zstr.avail_out == 0)
            {
                outBuf.append(_buffer, _buffer + INFLATE_BUFFER_SIZE);
                _zstr.next_out = (unsigned char *)_buffer;
                _zstr.avail_out = INFLATE_BUFFER_SIZE;
            }
            else if (_zstr.avail_in == 0)
            {
                outBuf.append(_buffer, _buffer + (INFLATE_BUFFER_SIZE - _zstr.avail_out));
                _zstr.next_out = (unsigned char *)_buffer;
                _zstr.avail_out = INFLATE_BUFFER_SIZE;
                break;
            }
        }
    }
    else
    {
         if (_zstr.next_out)
        {
            int rc = inflate(&_zstr, Z_FINISH);
            printf("%s(%d) ret:%d, avail_in:%u, avail_out:%u\n",
                   __FUNCTION__, __LINE__, rc, _zstr.avail_in, _zstr.avail_out);
            if (rc != Z_OK && rc != Z_STREAM_END)
            {
                perror("deflate ");
                return -1;
            }
            outBuf.append(_buffer, INFLATE_BUFFER_SIZE - _zstr.avail_out);
            _zstr.next_out = (unsigned char *)_buffer;
            _zstr.avail_out = INFLATE_BUFFER_SIZE;
            while (rc != Z_STREAM_END)
            {
                rc = deflate(&_zstr, Z_FINISH);
                printf("%s(%d) ret:%d, avail_in:%u, avail_out:%u\n",
                       __FUNCTION__, __LINE__, rc, _zstr.avail_in, _zstr.avail_out);
                if (rc != Z_OK && rc != Z_STREAM_END)
                {
                    perror("deflate ");
                    return -1;
                }
                outBuf.append(_buffer, INFLATE_BUFFER_SIZE - _zstr.avail_out);
                _zstr.next_out = (unsigned char *)_buffer;
                _zstr.avail_out = INFLATE_BUFFER_SIZE;
            }
        }
    }
    return 0;
}