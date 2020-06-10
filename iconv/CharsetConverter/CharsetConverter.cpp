#include <errno.h>
#include "CharsetConverter.h"
#include <stdio.h>
#include <stdlib.h>

CharsetConverter::CharsetConverter(const std::string &strFromCharset,
								   const std::string &strToCharset)
{
	cd = iconv_open(strFromCharset.c_str(), strToCharset.c_str());
	if ((iconv_t)-1 == cd)
		perror("iconv_open");
}

CharsetConverter::~CharsetConverter()
{
	if ((iconv_t)-1 != cd)
		iconv_close(cd);
}

int CharsetConverter::Convert(iconv_t iCd,
							  const std::string &strSrc,
							  std::string &strResult,
							  bool bErrContinue)
{
	unsigned int dwInLen = strSrc.size();
	char *pcInBuf = const_cast<char *>(strSrc.c_str());
	size_t dwInBytesLeft = dwInLen;
	size_t dwOutBytesLeft = dwInLen;
	char *pcOutBuf = NULL;
	char *pcDest = NULL;
	size_t ret = 0;
	int done = 0;
	int dwErrnoSaved = 0;
	int dwBufSize = dwInLen * 2;
	pcDest = new char[dwBufSize];
	strResult = "";
	//bErrContinue = 0;
	printf("Convert bErrContinue = %d\n", bErrContinue);
	while (dwInBytesLeft > 0 && !done) // 字符是否已经转换完，异常处理 done
	{
		pcOutBuf = pcDest;
		dwOutBytesLeft = dwBufSize;
		ret = iconv(iCd, &pcInBuf, &dwInBytesLeft, &pcOutBuf, &dwOutBytesLeft);
		printf("ret:%ld, dwInBytesLeft:%ld, dwOutBytesLeft:%ld\n", ret, dwInBytesLeft, dwOutBytesLeft);
		dwErrnoSaved = errno;
		if (pcDest != pcOutBuf) // 有没有字符输出
		{
			printf("append1 add %ld bytes\n", pcOutBuf - pcDest);
			strResult.append(pcDest, pcOutBuf - pcDest); // 加入转换好的字符
		}
		if (ret != (size_t)-1)
		{
			pcOutBuf = pcDest;
			dwOutBytesLeft = dwBufSize;
			(void)iconv(iCd, NULL, NULL, &pcOutBuf, &dwOutBytesLeft);
			if (pcDest != pcOutBuf) // 检测iconv内部是否还有缓存
			{
				printf("append2 add %ld bytes\n", pcOutBuf - pcDest);
				strResult.append(pcDest, pcOutBuf - pcDest);
			}
			dwErrnoSaved = 0;
			break;
		}

		switch (dwErrnoSaved) // ret == (size_t)-1 部分的处理
		{
		// 输出缓冲不够大
		case E2BIG:
			printf("E2BIG\n");
			break;
		// 遇到无效的字符
		case EILSEQ:
			printf("EILSEQ %d\n", bErrContinue);
			if (bErrContinue)
			{
				dwErrnoSaved = 0;
				// 跳过无效的字符
				dwInBytesLeft = dwInLen - (pcInBuf - strSrc.c_str()); // forward one illegal byte
				dwInBytesLeft--;
				pcInBuf++;
				printf("EILSEQ dwInBytesLeft = %ld, pcInBuf = %p\n", dwInBytesLeft, pcInBuf);
				break;
			}
			done = 1;
			break;
		case EINVAL:
			printf("EINVAL\n");
			done = 1;
			break;
		default:
			printf("UNkown\n");
			done = 1;
			break;
		}
	}
	delete[] pcDest;
	errno = dwErrnoSaved;
	return (dwErrnoSaved) ? -1 : 0;
}

int CharsetConverter::Convert(const std::string &strSrc,
							  std::string &strResult,
							  bool bErrContinue)
{
	return Convert(cd, strSrc, strResult, bErrContinue);
}

int CharsetConverter::Convert(const std::string &strFromCharset, const std::string &strToCharset,
							  const std::string &strSrc, std::string &strResult,
							  bool bErrContinue)
{
	iconv_t iCd = iconv_open(strFromCharset.c_str(), strToCharset.c_str());
	if ((iconv_t)-1 == iCd)
	{
		perror("iconv_open");
		return -1;
	}

	return Convert(iCd, strSrc, strResult, bErrContinue);
}
// iconv_t iconv_open (const char* tocode, const char* fromcode);
CUnicodeToGbk::CUnicodeToGbk()
{
	cd = iconv_open("UNICODE", "GBK");
	if ((iconv_t)-1 == cd)
		perror("iconv_open");
}

CUtf8ToGbk::CUtf8ToGbk()
{
	cd = iconv_open("GBK", "UTF-8");
	if ((iconv_t)-1 == cd)
		perror("iconv_open");
}
//iconv_t iconv_open (const char* tocode, const char* fromcode);
CUtf8ToUtf16::CUtf8ToUtf16()
{
	cd = iconv_open("UTF-16", "UTF-8");
	if ((iconv_t)-1 == cd)
		perror("iconv_open");
}

CUtf16ToGbk::CUtf16ToGbk()
{
	cd = iconv_open("GBK", "UTF-16");
	if ((iconv_t)-1 == cd)
		perror("iconv_open");
}

CUtf16ToUtf8::CUtf16ToUtf8()
{
	cd = iconv_open("UTF-8", "UTF-16");
	if ((iconv_t)-1 == cd)
		perror("iconv_open");
}

CUtf16BEToGbk::CUtf16BEToGbk()
{
	cd = iconv_open("GBK", "UTF-16BE");
	if ((iconv_t)-1 == cd)
		perror("iconv_open");
}

CBig5ToGbk::CBig5ToGbk()
{
	cd = iconv_open("GBK", "BIG5");
	if ((iconv_t)-1 == cd)
		perror("iconv_open");
}

CISO8859ToGbk::CISO8859ToGbk()
{
	cd = iconv_open("GBK", "ISO-8859-1");
	if ((iconv_t)-1 == cd)
		perror("iconv_open");
}

CHzToGbk::CHzToGbk()
{
	cd = iconv_open("GBK", "HZ");
	if ((iconv_t)-1 == cd)
		perror("iconv_open");
}

CGbkToUtf8::CGbkToUtf8()
{
	cd = iconv_open("UTF-8", "GBK");
	if ((iconv_t)-1 == cd)
		perror("iconv_open");
}
CGb2312ToGbk::CGb2312ToGbk()
{
	cd = iconv_open("GBK", "GB2312");
	if ((iconv_t)-1 == cd)
		perror("iconv_open");
}

CUtf8ToGb2312::CUtf8ToGb2312()
{
	cd = iconv_open("GB2312", "UTF-8");
	if ((iconv_t)-1 == cd)
		perror("iconv_open");
}

CGb2312ToUtf8::CGb2312ToUtf8()
{
	cd = iconv_open("UTF-8", "GB2312");
	if ((iconv_t)-1 == cd)
		perror("iconv_open");
}
