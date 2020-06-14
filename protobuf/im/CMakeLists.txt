cmake_minimum_required(VERSION 2.6)

project (im)

ADD_DEFINITIONS( -g -W -Wall -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DAC_HAS_INFO
-DAC_HAS_WARNING -DAC_HAS_ERROR -DAC_HAS_CRITICAL -DTIXML_USE_STL
-DAC_HAS_DEBUG -DLINUX_DAEMON -std=c++11)

set(im_srcs
    msg.pb.cc 
    IM.BaseDefine.pb.cc
    IM.Login.pb.cc
    IM.Message.pb.cc
    UtilPdu.cc
    ImPduBase.cc
    util.cc
    Lock.cc
)

set(server_srcs
server.cc
)

set(client_srcs
client.cc
)

add_executable(server_im ${im_srcs} ${server_srcs})
add_executable(client_im ${im_srcs} ${client_srcs})
TARGET_LINK_LIBRARIES(server_im protobuf pthread)
TARGET_LINK_LIBRARIES(client_im protobuf pthread)





