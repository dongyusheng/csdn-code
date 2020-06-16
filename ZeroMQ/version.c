//version.c
//https://blog.csdn.net/qq_41453285/article/details/105990320
//https://github.com/dongyusheng/csdn-code/edit/master/ZeroMQ/version.c
#include <stdio.h>
#include <zmq.h>

int main()
{
    int major, minor, patch;
    zmq_version(&major, &minor, &patch);
    printf("Current ZeroMQ version is %d.%d.%d\n", major, minor, patch);
}
