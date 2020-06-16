//version.c
#include <stdio.h>
#include <zmq.h>

int main()
{
    int major, minor, patch;
    zmq_version(&major, &minor, &patch);
    printf("Current ZeroMQ version is %d.%d.%d\n", major, minor, patch);
}
