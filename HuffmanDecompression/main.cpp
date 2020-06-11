//使用哈夫曼树压缩和解压文档
/*
 * @Author: your name
 * @Date: 2020-06-04 16:21:01
 * @LastEditTime: 2020-06-04 19:13:01
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \1-huffman\main.cpp
 */ 
/*利用库中的优先级队列实现哈夫曼树，最后基于哈夫曼树最终实现文件压缩。 
描述： 
    1.统计文件中字符出现的次数，利用优先级队列构建Haffman树，生成Huffman编码。 
    构造过程可以使用priority_queue辅助，每次pq.top()都可以取出权值（频数）最小的节点。每取出两个最小权值的节点，就new出一个新的节点，左右孩子分别指向它们。然后把这个新节点push进优先队列。  
    2.压缩：利用Haffman编码对文件进行压缩，即在压缩文件中按顺序存入每个字符的Haffman编码。 
    3.将文件中出现的字符以及它们出现的次数写入配置文件中，以便后续压缩使用。 
    4.解压缩：利用配置文件重构Haffman树，对文件进行减压缩。
*/

#include "FileCompress.hpp"
#include <iostream>
#include <vector>

#include <assert.h>

// 注意是用g++编译器
// 编译 g++ -o huffman main.cpp HuffmanTree.hpp FileCompress.hpp 
int main(int argc, char** argv)
{
    if (argc != 2)
	{
		std::cout << "usage: " << argv[0] << ": <input_file> " << std::endl;
		return 1;
	}


    FileCompress fc;
    FileCompress fc1;
    std::string fileName(argv[1]);

    fc.Compress(fileName.c_str());      // 压缩文件
    fileName += ".huffman";             // 压缩后的文件名
    fc1.Uncompress(fileName.c_str());   // 解压缩

    return 0;
}