#!/bin/sh
SRC_DIR=./
DST_DIR=./

#C++
mkdir -p $DST_DIR/cpp
protoc -I=$SRC_DIR --cpp_out=$DST_DIR $SRC_DIR/*.proto


