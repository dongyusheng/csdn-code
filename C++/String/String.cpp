#include "String.h"

String::String()
{
    data = new char[1];
    *data = '\0';
    size = 0;
}

String::String(int n, char c)
{
    data = new char[n + 1];
    char *temp = data;

    while(n--)
        *temp++ = c;

    *temp = '\0';
    size = n;
}

String::String(const char * source)
{
    if(source == NULL)
    {
        data = new char[1];
        *data = '\0';
        size = 0;
        return;
    }
    
    size  = strlen(source);
    data = new char[size + 1];

    char *temp = data;

    while(*source != '\0')
        *temp++ = *source++;

    *temp = '\0';
}

String::String(const String& s)
{
    if(s.size == 0)
    {
        data = new char[1];
        *data='\0';
        size = 0;
        return;
    }

    data = new char[s.size + 1];

    char *temp1 = data;
    char *temp2 = s.data;

    while(*temp2 != '\0')
        *temp1++ = *temp2++;
    *temp1= '\0';

    size = s.size;
    
}

String::~String()
{
    if(data)
    {
        delete[] data;
        data = nullptr;
        size = 0;
    }
}

String& String::operator=(const char* s)
{
    if(data)
    {
        delete[] data;
    }

    if(s == NULL)
    {
        data = new char[1];
        *data = '\0';
        size = 0;
    }

    size = strlen(s);
    data = new char[size + 1];

    char *temp = data;
    
    while(*s != '\0')
        *temp++ = *s++;

    *temp = '\0';

    return *this;
}

String& String::operator=(const String& s)
{
    if(this == &s)
        return *this;

    if(data)
        delete[] data;

    size = s.size;
    
    data = new char[size + 1];

    char *temp1 = s.data;
    char *temp2 = data;
    
    while(*temp1 != '\0')
        *temp2++ = *temp1++;
    *temp2 = '\0';

    return *this;
}

char& String::operator[](int i)
{
    return data[i];
}

const char& String::operator[](int i)const
{
    return data[i];
}

String& String::operator+=(const String& s)
{
    if(s.size == 0)
        return *this;

    char *temp = new char[size + s.size + 1];
    memcpy(temp, data, size);

    memcpy(temp + size, s.data, s.size);
    *(temp + size + s.size + 1) = '\0';

    if(data != NULL)
        delete[] data;
    data = temp;
    size += s.size;

    return *this;
}

String& String::operator+=(const char *s)
{
    if(s == NULL || (strlen(s) == 0))
        return *this;

    char *temp = new char[size + strlen(s) + 1];
    memcpy(temp, data, size);
    memcpy(temp + size, s, strlen(s));
    *(temp + size + strlen(s) + 1) = '\0';

    if(data != NULL)
        delete[] data;
    data = temp; 
    size += strlen(s);

    return *this;
}
 
ostream& operator<<(ostream& out, const String& s)
{
    out << s.data;
    return out;
}

istream& operator>>(istream& in, const String& s)
{
    char p[50];
    in.getline(p, 50);
    
    s = p; // 调用赋值运算符
    
    return in;
}

bool operator<(const String& s1, const String& s2)
{
    int i = 0;
    while(s1[i] == s2[i] && s1[i] != '\0' && s2[i] != '\0')
        i++;

    return (s1[i] < s2[i]) ? true : false;
} 

bool operator>(const String& s1, const String& s2)
{
    return !(s1 < s2);
}

bool operator<=(const String& s1, const String& s2)
{
    int i = 0;
    while(s1[i] == s2[i] && s1[i] != '\0' && s2[i] != '\0')
        i++;
    
    return (s1[i] < s2[i] || (i == s1.size)) ? true : false;
}


bool operator>=(const String& s1, const String& s2)
{
    return !(s1 < s2);
}

bool operator==(const String& s1, const String& s2)
{
    char *temp1 = s1.data;
    char *temp2 = s2.data;

    if(s1.size != s2.size)
        return false;

    while((*temp1++ == *temp2++) && (*temp1 != '\0') && (*temp2 != '\0'));

    if(*temp1 == '\0')
        return true;
}

bool operator!=(const String& s1, const String& s2)
{
    return !(s1 == s2);
}
