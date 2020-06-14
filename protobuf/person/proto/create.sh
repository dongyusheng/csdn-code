#!/bin/sh
# proto文件在哪里
SRC_DIR=./
# .h .cc输出到哪里
DST_DIR=../

#C++
protoc -I=$SRC_DIR --cpp_out=$DST_DIR $SRC_DIR/*.proto
