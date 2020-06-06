//queue_cas_test.cpp
#include "queue_cas.h"

int main()
{
    Queue<int> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);
    queue.show();

    queue.pop();
    queue.show();

    queue.pop();
    queue.show();

    queue.pop();
    queue.show();

    queue.push(1);
    queue.show();

    queue.push(2);
    queue.show();
}
