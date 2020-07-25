// main.cpp
#include "SmartPtr.h"

class test
{
public:
    test(char *_name = ""): name(_name) {}

    void showName() { cout << name << endl; }
private:
    string name;
};

int main()
{
    SmartPtr<test> mp1;

    SmartPtr<test> mp2(new test("C++"));
    SmartPtr<test> mp3(mp2);
    mp2->showName();
    mp3->showName();
    std::cout << std::endl;

    SmartPtr<test> mp4(new test("Linux"));
    mp3 = mp4;
    mp3->showName();
    mp4->showName();
    std::cout << std::endl;

    *mp3 = "Golang";
     mp3->showName();
    
    return 0;
}
