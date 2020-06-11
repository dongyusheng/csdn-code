#include <iostream>
#include <fstream>
#include <stdio.h>

#include "InflatingStream.h"

#define DATA_SIZE 1024
int decompressOneFile(char *infilename, char *outfilename)
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

    InflatingStream *infalte = NULL;
    try
    {
        infalte = new InflatingStream(InflatingStream::STREAM_GZIP);
    }
    catch (const char* c)
	{
		printf(" new InflatingStream failed, it is %s \n", c);
        if(infalte)
            delete infalte;
        return -1;
	}
  
    while ((numRead = fread(inbuffer, 1, sizeof(inbuffer), infile)) > 0)
    {
        outStr.clear();
        int ret = infalte->Inflate(inbuffer, numRead, outStr);
        printf("%s(%d) ret:%d, outStr size:%ld\n", __FUNCTION__, __LINE__, ret, outStr.size());
        fwrite(outStr.c_str(), outStr.size(), 1, outfile);
    }
    outStr.clear();
    int ret = infalte->Inflate(NULL, 0, outStr);
    printf("%s(%d) ret:%d, outStr size:%ld\n",  __FUNCTION__, __LINE__, ret, outStr.size());
    fwrite(outStr.c_str(), outStr.size(), 1, outfile);

    fclose(infile);
    fclose(outfile);

    delete infalte;
    
    return 0;
}

// 编译方式 g++ -o inflate InflatingStream.cpp inflate.cpp -lz
int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "usage: " << argv[0] << ": <input_file> <output_file>" << std::endl
		          << "       read <input_file>, deflate (compress) it and write the result to <output_file>" << std::endl;
		return 1;
	}

	decompressOneFile(argv[1], argv[2]);
	
	return 0;
}
