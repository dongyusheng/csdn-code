#ifndef __STRING_H_
#define __STRING_H_

#include <iostream>
#include <string.h>

using namespace std;

class String
{
public:
    String();
    String(int n, char c);
    String(const char* source);
    String(const String& s);

    ~String();
public:
    String& operator=(char* s);
    String& operator=(const String& s);
    
    char& operator[](int i);
    const char& operator[](int i)const;

    String& operator+=(const String& s);
    String& operator+=(const char *s);
    
    friend ostream& operator<<(ostream& out, const String& s);
    friend istream& operator>>(istream& in, const String& s);

    friend bool operator<(const String& s1, const String& s2);
    friend bool operator>(const String& s1, const String& s2);
    friend bool operator<=(const String& s1, const String& s2);
    friend bool operator>=(const String& s1, const String& s2);

    friend bool operator==(const String& s1, const String& s2);
    friend bool operator!=(const String& s1, const String& s2);
public:
    char* getData() { return data; }
    int length() { return size; }
private:
    int size;
    char *data;
};

#endif // __STRING_H_
