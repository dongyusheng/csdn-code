#include "DeflatingStream.h"
#include <iostream>
#include <fstream>
#include <stdio.h>


#define DATA_SIZE 1024
int compressOneFile(char *infilename, char *outfilename)
{
    int numRead = 0;
    char inbuffer[DATA_SIZE] = {0};
    FILE *infile = fopen(infilename, "rb");
    FILE *outfile = fopen(outfilename, "wb");
    std::string outStr; 
    if (!infile || !outfile )
    {
        return -1;
    }

    DeflatingStream *defalte = NULL;
    try
    {
        defalte = new DeflatingStream(STREAM_GZIP, Z_DEFAULT_COMPRESSION, std::string(infilename));
    }
    catch (const char* c)
	{
		printf(" new DeflatingStream failed, it is %s \n", c);
        if(defalte)
            delete defalte;
        return -1;
	}
    
    while ((numRead = fread(inbuffer, 1, sizeof(inbuffer), infile)) > 0)
    {
        outStr.clear();
        int ret = defalte->Deflate(inbuffer, numRead, outStr); // 压缩数据
        printf("%s(%d) ret:%d, outStr size:%ld\n", __FUNCTION__, __LINE__, ret, outStr.size());
        // 写入压缩数据
        if(outStr.size() > 0)
            fwrite(outStr.c_str(), outStr.size(), 1, outfile);
    }
    outStr.clear();
    int ret = defalte->Deflate(NULL, 0, outStr);
    printf("%s(%d) ret:%d, outStr size:%ld\n",  __FUNCTION__, __LINE__, ret, outStr.size());
    fwrite(outStr.c_str(), outStr.size(), 1, outfile);

    fclose(infile);
    fclose(outfile);

    delete defalte;
    
    return 0;
}
// linux解压命令 gunzip 1.gz

//编译方式 g++ -o deflate deflate.cpp DeflatingStream.cpp -lz
int main(int argc, char** argv)
{
	if (argc != 3)
	{
		//压缩的格式必须为.gz
		std::cout << "usage: " << argv[0] << ": <input_file> <output_file.gz>" << std::endl
		          << "       read <input_file>, deflate (compress) it and write the result to <output_file>" << std::endl;
		return 1;
	}

	compressOneFile(argv[1], argv[2]);
	
	
	return 0;
}
