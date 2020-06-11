/*利用库中的优先级队列实现哈夫曼树，最后基于哈夫曼树最终实现文件压缩。 
描述： 
    1.统计文件中字符出现的次数，利用优先级队列构建Haffman树，生成Huffman编码。 
    构造过程可以使用priority_queue辅助，每次pq.top()都可以取出权值（频数）最小的节点。每取出两个最小权值的节点，就new出一个新的节点，左右孩子分别指向它们。然后把这个新节点push进优先队列。  
    2.压缩：利用Haffman编码对文件进行压缩，即在压缩文件中按顺序存入每个字符的Haffman编码。 
    3.将文件中出现的字符以及它们出现的次数写入配置文件中，以便后续压缩使用。 
    4.解压缩：利用配置文件重构Haffman树，对文件进行减压缩。
*/
#ifndef _FILE_COMPRESS_H_
#define _FILE_COMPRESS_H_
#include "HuffmanTree.hpp"
#include <iostream>
#include <vector>
#include <assert.h>

using namespace std;


unsigned long getFileSize(const char *path)
{
	unsigned long filesize = -1;
	FILE *fp;
	fp = fopen(path, "r");
	if(fp == NULL)
		return filesize;
	fseek(fp, 0L, SEEK_END);
	filesize = ftell(fp);
	fclose(fp);
	return filesize;
}


typedef unsigned int CountType;
typedef unsigned char CharType;
struct CharInfo
{
    CharType _ch;//字符
    CountType _count;//次数
    string  _code;//编码

    bool operator!=(const CharInfo &info)
    {
        return _count != info._count;
    }
    CharInfo operator+(const CharInfo &info)
    {
        CharInfo ret;
        ret._count = _count + info._count;
        return ret;
    }
    bool operator<(const CharInfo &info)
    {
        return _count > info._count;
    }
};

class FileCompress
{
    typedef HuffmanTreeNode<CharInfo> Node;
    struct TmpInfo
    {
        CharType _ch;//字符
        CountType _count;//次数
    };

public:
    //构造函数
    FileCompress()
    {
        for (size_t i = 0; i < 256; ++i)
        {
            _infos[i]._ch = i;
            _infos[i]._count = 0;
        }
    }
    //获取哈夫曼编码
    void GenerateHuffmanCode(Node *root, string code) 
    {
        if (root == NULL)
            return;
        //前序遍历生成编码
        if (root->_pLeft == NULL && root->_pRight == NULL)
        {
            _infos[(unsigned char)root->_weight._ch]._code = code;
            return;
        }
        GenerateHuffmanCode(root->_pLeft, code + '0');
        GenerateHuffmanCode(root->_pRight, code + '1');
    }
    // 真正的压缩算法窗口是32k, 我们这里直接对所有数据进行编码 
    void Compress(const char *file) //file:源文件
    {
        unsigned long fileSize = getFileSize(file);
        //1.统计字符出现的次数
        FILE *fout = fopen(file, "rb");
        assert(fout);
        char ch = fgetc(fout);
        while (feof(fout) == 0) //如文件结束，则返回值为1，否则为0
        {
            _infos[(unsigned char)ch]._count++;     // 计算对应字符出现的频率
            ch = fgetc(fout);
        }
        //2.生成Huffmantree 及code
        CharInfo invalid;
        invalid._count = 0;
        // 2.1 生成Huffmantree， 构建哈夫曼树
        HuffmanTree<CharInfo> tree(_infos, 256, invalid); //参数：数组，256个，无效值（出现0次）

        string compressfile = file;                    //
        compressfile += ".huffman";                    //?
        FILE *fin = fopen(compressfile.c_str(), "wb"); //打开压缩文件
        assert(fin);

        string code;
        //  2.2 生成code
        GenerateHuffmanCode(tree.GetRoot(), code);
        //3.0 写入字符出现的信息
        int writeNum = 0;
        int objSize = sizeof(TmpInfo);
        for (size_t i = 0; i < 256; ++i)            // 这里实质是讲码表存储到文件的头部
        {
            if (_infos[i]._count > 0) 
            {
                TmpInfo info;
                info._ch = _infos[i]._ch;
                info._count = _infos[i]._count;
                printf("codec ch:%u, cout:%u\n", (unsigned char)info._ch, info._count);
                fwrite(&info, objSize, 1, fin);         // 将对应的码流信息写入
                writeNum++;
            }
        }
        TmpInfo info;
        info._count = -1;
        printf("code objSize:%d\n", objSize);
        fwrite(&info, objSize, 1, fin); //把info._count = -1写进去作为结束标志位
        

        //3.压缩
        int writeCount = 0;
        int readCount = 0;
        fseek(fout, 0, SEEK_SET); //文件指针、偏移量、参照位置
        ch = fgetc(fout);
        readCount++;
        unsigned char value = 0;
        size_t pos = 0;

        while (feof(fout) == 0)           // 一个个字符读取,效率虽然低一些，但不影响实验
        {
            // 读取数据，查找对应编码
            string &code = _infos[(unsigned char)ch]._code; // 查找对应的编码
            printf("code[%d]:%u:%s\n", readCount, (unsigned char)ch, code.c_str());
            // code 实际存储的是对应 某个字符的哈夫曼编码
            for (size_t i = 0; i < code.size(); ++i)
            {
                if (code[i] == '1')
                    value |= (1 << pos);
                else if (code[i] == '0')
                {
                    value &= ~(1 << pos);
                }
                else
                {
                    assert(false);
                    printf("assert(false); ??????????");
                }
                ++pos;
                if (pos == 8)
                {
                    writeCount++;
                    fputc(value, fin);      // 够8个bit存储
                    value = 0;
                    pos = 0;
                }
            }
            readCount++;
            ch = fgetc(fout);
        }

        if (pos > 0)
        {
            writeCount++;
            fputc(value, fin); //写入压缩文件（fin）
        }
        printf("huffman code table  size::%d\n", objSize * (writeNum + 1));
        printf("compress file data  size:%d\n", writeCount);
        unsigned long totalSize = writeCount + objSize *  (writeNum + 1);
        printf("compress file total size:%lu, orign file size:%lu, ratio:%0.2f\n",
             totalSize, fileSize,  (float)(totalSize*1.0/fileSize));
        fclose(fout);
        fclose(fin);
    }
    void Uncompress(const char *file)
    {
        string uncompressfile = file;           //file:Input.txt.huffman
        size_t pos = uncompressfile.rfind('.'); //找到倒数第一个'.'
        assert(pos != string::npos);
        uncompressfile.erase(pos);                       //删除掉'.'后面字符串
        uncompressfile += ".unhuffman";                  //Input.txt+'.unhuffman'
        FILE *fin = fopen(uncompressfile.c_str(), "wb"); //打开解压缩文件
        assert(fin);
        FILE *fout = fopen(file, "rb"); //打开压缩文件
        assert(fout);

        //3.0 读入字符出现的信息
        TmpInfo info;
        int cycleNum = 1;
        int objSize = sizeof(TmpInfo);
        fread(&info, objSize, 1, fout);

        while (info._count != -1)           //-1为结束标志 
        {
            //  printf("decodec ch:%u, cout:%u\n", (unsigned char)info._ch, info._count);
            _infos[(unsigned char)info._ch]._ch = info._ch;
            _infos[(unsigned char)info._ch]._count = info._count;

            fread(&info, objSize, 1, fout);
            cycleNum++;
        }

        int aaa = 0;
        //重建huaffman树
        CharInfo invalid;
        invalid._count = 0;
        HuffmanTree<CharInfo> tree(_infos, 256, invalid); //参数：数组，256个，无效值（出现0次）
        Node *root = tree.GetRoot();
        Node *cur = root;
        CountType n = root->_weight._count; //所有叶子节点的和（源文件字符的个数）
        char ch = fgetc(fout);             //从fout(压缩文件)读字符
        int readCount = 0;

        while (ch != EOF || n > 0) //还需要按bit去查找哈夫曼编码对应的字符
        {
			//对单个字节去编码(8bit)
            for (size_t i = 0; i < 8; ++i)
            {
                if ((ch & (1 << i)) == 0)//等于0往左边找,通过哈夫曼结构图可以看出根节点的左节点对应的二进制为0
                    cur = cur->_pLeft;   
                else
                    cur = cur->_pRight; // 往右边找
                if (cur->_pLeft == NULL && cur->_pRight == NULL)
                {
                    //cout << cur->_weight._ch;
                    readCount++;
                    if(readCount % 1024 == 0) 
                    {
                        // printf("uncompress %dk data\n", readCount/1024);
                    }
					//查找到对应的字符才写入
                    fputc(cur->_weight._ch, fin); //fin解压缩文件
                    cur = root;
                    if (--n == 0)
                        break;
                }
            }
            ch = fgetc(fout);
        }
        printf("uncompress end\n");
        fclose(fin);
        fclose(fout);
    }

protected:
    CharInfo _infos[256];
};


#endif