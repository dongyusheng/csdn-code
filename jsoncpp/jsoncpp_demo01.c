//https://github.com/dongyusheng/csdn-code/tree/master/jsoncpp
#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

int main()
{
    std::cout << "Hello World!" << std::endl;

    Json::Value root;
    Json::FastWriter fast;

    root["ModuleType"] = Json::Value(1);
    root["ModuleCode"] = Json::Value("China");

    std::cout<<fast.write(root)<<std::endl;

    Json::Value sub;
    sub["version"] = Json::Value("1.0");
    sub["city"] = Json::Value(root);
    fast.write(sub);

    std::cout<<sub.toStyledString()<<std::endl;

    return 0;
}
