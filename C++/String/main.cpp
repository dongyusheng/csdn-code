#include "String.h"

int main()
{
    std::cout << "********构造函数********" << std::endl;

    String s1;
    std::cout << "s1:" << s1 << std::endl;
    
    String s2(10, 'a');
    std::cout << "s2:" << s2 << std::endl;

    String s3("HelloWorld");
    std::cout << "s3:" << s3 << std::endl << std::endl;



    std::cout << "********拷贝构造********" << std::endl;

    String s4(s2);
    std::cout << "s4:" << s4 << std::endl;

    String s5 = s3;
    std::cout << "s5:" << s5 << std::endl << std::endl;



    std::cout << "********=运算符********" << std::endl;

    s4 = "bbb";
    std::cout << "s4:" << s4 << std::endl;

    s4 = s5;
    std::cout << "s4:" << s4 << std::endl << std::endl;



    std::cout << "********下标运算符********" << std::endl;
    
    std::cout <<"s4[0]: " <<s4[0] << std::endl;
    std::cout <<"s4[1]: " <<s4[1] << std::endl;
    std::cout <<"s4[9]: " <<s4[9] << std::endl << std::endl;


    std::cout << "********+=运算符********" << std::endl;

    s4 += s5;
    std::cout << "s4:" << s4 << std::endl;
    s4 += ",C++";
    std::cout << "s4:" << s4 << std::endl;

    s5 += s4;
    std::cout << "s5:" << s5 << std::endl;
    s5 += ",C";
    std::cout << "s5:" << s5 << std::endl << std::endl;



    std::cout << "********>、<、>=、<=运算符********" << std::endl;

    String s6("abc");
    String s7("abcd");
    String s8("axc");

    std::cout << "s6 > s7: " << (s6 > s7) <<std::endl;
    std::cout << "s6 < s7: " << (s6 < s7) <<std::endl;
    std::cout << "s6 >= s7: " << (s6 >= s7) <<std::endl;
    std::cout << "s6 <= s7: " << (s6 <= s7) <<std::endl;

    std::cout << "s6 > s8: " << (s6 > s8) <<std::endl;
    std::cout << "s6 < s8: " << (s6 < s8) <<std::endl;
    std::cout << "s6 >= s8: " << (s6 >= s8) <<std::endl;
    std::cout << "s6 <= s8: " << (s6 <= s8) <<std::endl <<std::endl;



    std::cout << "********==、!=运算符********" << std::endl;

    
    String s9("abc");
    String s10("axcd");
    String s11("abcd");
    String s12("abcd");

    std::cout << "s9 == s10: " << (s9 == s10) <<std::endl;
    std::cout << "s9 != s10: " << (s9 != s10) <<std::endl;
    std::cout << "s9 == s11: " << (s9 == s11) <<std::endl;
    std::cout << "s9 != s11: " << (s9 != s11) <<std::endl;

    std::cout << "s11 == s12: " << (s11 == s12) <<std::endl;
    std::cout << "s11 != s12: " << (s11 != s12) <<std::endl <<std::endl;


    std::cout << "********>>运算符********" << std::endl;

    String s13;
    std::cout << "Please write to s13: ";
    cin >> s13;
    std::cout << "s13: " << s13 <<std::endl;

    
    return 0;
}
