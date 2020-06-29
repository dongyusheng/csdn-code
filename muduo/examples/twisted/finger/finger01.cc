// finger01.cc
// https://blog.csdn.net/qq_41453285/article/details/107015436
#include "muduo/net/EventLoop.h"

using namespace muduo;
using namespace muduo::net;

int main()
{
    EventLoop loop;
    loop.loop();
}
