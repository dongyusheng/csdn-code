//测试jsoncpp的使用方法和性能
#include <memory>
#include <sstream>
#include <iostream>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "jsoncpp/json/json.h"
#include "jsoncpp/json/value.h"
#include "jsoncpp/json/reader.h"
#include "jsoncpp/json/writer.h"

/*

{
  "name": "milo",
  "age": 80,
  "languages": ["C++", "C"],
  "phone": {
    "number": "186****3143",
    "type": "home"
  },
  "books":[
      {
          "name": "Linux kernel development",
          "price":7.7
      },
      {
          "name": "Linux server development",
          "price": 8.0
      }
  ]，
  "vip":true,
  "address": null
}
 */

static uint64_t getNowTime()
{
    struct timeval tval;
    uint64_t nowTime;

    gettimeofday(&tval, NULL);

    nowTime = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
    return nowTime;
}

std::string JsoncppEncodeNew()
{
    std::string jsonStr;
    // 一个value是可以包含多个键值对
    Json::Value root, languages, phone, book, books;

    // 姓名
    root["name"] = "milo";
    // 年龄
    root["age"] = 80;

    // 语言
    languages[0] = "C++";
    languages[1] = "Java";
    root["languages"] = languages;

    // 电话
    phone["number"] = "186****3143";
    phone["type"] = "home";
    root["phone"] = phone;

    // 书籍
    book["name"] = "Linux kernel development";
    book["price"] = 7.7;
    books[0] = book;    // 深拷贝
    book["name"] = "Linux server development";
    book["price"] = 8.0;
    books[1] = book;    // 深拷贝
    root["books"] = books;

    // 是否为vip
    root["vip"] = true;

    // address信息空null
    root["address"] = "yageguoji";

    Json::StreamWriterBuilder writerBuilder;
    std::ostringstream os;
    std::unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    jsonWriter->write(root, &os);
    jsonStr = os.str();

    // std::cout << "Json:\n" << jsonStr << std::endl;
    return jsonStr;
}

std::string JsoncppEncodeOld()
{
    std::string jsonStr;
    Json::Value root, languages, phone, book, books;

    // 姓名
     root["name"] = "milo";
    //root["name"] = Json::nullValue;
    // 年龄
    root["age"] =  80; 

    // 语言
    languages[0] = "C++";
    languages[1] = "Java";
    root["languages"] = languages;

    // 电话
    phone["number"] = "186****3143";
    phone["type"] = "home";
    root["phone"] = phone;

    // 书籍
    book["name"] = "Linux kernel development";
    book["price"] = (float)7.7;
    books[0] = book;
    book["name"] = "Linux server development";
    book["price"] = (float)8.0;
    books[1] = book;
    root["books"] = books;

    // 是否为vip
    root["vip"] = true;

    // address信息空null
    root["address"] = "yageguoji"; //;// Json::nullValue; // 如果是null，则要用自定义的Json::nullValue，不能用NULL

    Json::FastWriter writer;
    jsonStr = writer.write(root);

    // std::cout << "Json:\n" << jsonStr << std::endl;
    return jsonStr;
}

// 不能返回引用
Json::Value JsoncppEncodeOldGet()
{
    Json::Value root;
    Json::Value languages, phone, book, books;

    // 姓名
    root["name"] = "milo";
    //root["name"] = Json::nullValue;
    // 年龄
    root["age"] =  80; 

    // 语言
    languages[0] = "C++";
    languages[1] = "Java";
    root["languages"] = languages;

    // 电话
    phone["number"] = "186****3143";
    phone["type"] = "home";
    root["phone"] = phone;

    // 书籍
    book["name"] = "Linux kernel development";
    book["price"] = (float)7.7;
    books[0] = book;
    book["name"] = "Linux server development";
    book["price"] = (float)8.0;
    books[1] = book;
    root["books"] = books;

    // 是否为vip
    root["vip"] = true;

    // address信息空null
    root["address"] = Json::nullValue; //"yageguoji";// Json::nullValue; // 如果是null，则要用自定义的Json::nullValue，不能用NULL

 

    std::cout << "JsoncppEncodeOldGet:\n"  << std::endl;
    return root;
}

void printJsoncpp(Json::Value &root)
{
    if(root["name"].isNull())
    {
        std::cout << "name null\n";
    }
    std::cout << "name: " << root["name"].asString() << std::endl;
    std::cout << "age: " << root["age"].asInt() << std::endl;

    Json::Value &languages = root["languages"];
    std::cout << "languages: ";
    for (uint32_t i = 0; i < languages.size(); ++i)
    {
        if (i != 0)
        {
            std::cout << ", ";
        }
        std::cout << languages[i].asString();
    }
    std::cout << std::endl;

    Json::Value &phone = root["phone"];
    std::cout << "phone number: " << phone["number"].asString() << ", type: " << phone["type"].asString() << std::endl;

    Json::Value &books = root["books"];
    for (uint32_t i = 0; i < books.size(); ++i)
    {
        Json::Value &book = books[i];
        std::cout << "book name: " << book["name"].asString() << ", price: " << book["price"].asFloat() << std::endl;
    }
    std::cout << "vip: " << root["vip"].asBool() << std::endl;

    if (!root["address"].isNull())
    {
        std::cout << "address: " << root["address"].asString() << std::endl;
    }
    else
    {
        std::cout << "address is null" << std::endl;
    }
}

bool JsoncppDecodeNew(const std::string &info)
{
    if (info.empty())
        return false;

    bool res;
    JSONCPP_STRING errs;
    Json::Value root;
    Json::CharReaderBuilder readerBuilder;
    std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());
    res = jsonReader->parse(info.c_str(), info.c_str() + info.length(), &root, &errs);
    if (!res || !errs.empty())
    {
        std::cout << "parseJson err. " << errs << std::endl;
        return false;
    }
    // printJsoncpp(root);

    return true;
}

bool JsoncppDecodeOld(const std::string &strJson)
{
    if (strJson.empty())
        return false;

    bool res;

    Json::Value root;
    Json::Reader jsonReader;

    res = jsonReader.parse(strJson, root);
    if (!res)
    {
        std::cout << "jsonReader.parse err. " << std::endl;
        return false;
    }

    // printJsoncpp(root);

    return true;
}

const char *strCjson = "{                 \
	\"name\":	\"milo\",         \
	\"age\":	80,                 \
	\"languages\":	[\"C++\", \"C\"],\
	\"phone\":	{                       \
		\"number\":	\"186****3143\",    \
		\"type\":	\"home\"            \
	},                                  \
	\"books\":	[{                         \
			\"name\":	\"Linux kernel development\",   \
			\"price\":	7.700000        \
		}, {                            \
			\"name\":	\"Linux server development\",   \
			\"price\":	8               \
		}],                             \
	\"vip\":	true,                   \
	\"address\":	null                \
}                                      \
";
#define TEST_COUNT 100000
int main()
{
    std::string jsonStrNew;
    std::string jsonStrOld;

    jsonStrNew = JsoncppEncodeNew();
    // JsoncppEncodeNew size: 337
    std::cout << "JsoncppEncodeNew size: " << jsonStrNew.size() << std::endl;
    std::cout << "JsoncppEncodeNew string: " << jsonStrNew << std::endl;
    JsoncppDecodeNew(jsonStrNew);

    jsonStrOld = JsoncppEncodeOld();
    // JsoncppEncodeOld size: 248
    std::cout << "\n\nJsoncppEncodeOld size: " << jsonStrOld.size() << std::endl;
    std::cout << "JsoncppEncodeOld string: " << jsonStrOld << std::endl;
    JsoncppDecodeOld(jsonStrOld);

    Json::Value root = JsoncppEncodeOldGet();
    Json::FastWriter writer;
    std::cout << "writer:\n"  << std::endl;
    std::string jsonStr = writer.write(root);
    std::cout << "\n\njsonStr string: " << jsonStrOld << std::endl;
#if 1
    uint64_t startTime;
    uint64_t nowTime;
    
    startTime = getNowTime();
    std::cout << "jsoncpp encode time testing" << std::endl;
    for(int i = 0; i < TEST_COUNT; i++)
    {
        JsoncppEncodeNew();
    }
    nowTime = getNowTime();
    std::cout << "jsoncpp encode " << TEST_COUNT << " time, need time: "
         << nowTime-startTime << "ms"  << std::endl;

    startTime = getNowTime();
    std::cout << "\njsoncpp encode time testing" << std::endl;
    for(int i = 0; i < TEST_COUNT; i++)
    {
        JsoncppEncodeOld();
    }
    nowTime = getNowTime();
    std::cout << "jsoncpp encode " << TEST_COUNT << " time, need time: "
         << nowTime-startTime << "ms"  << std::endl;

    startTime = getNowTime();
    std::cout << "\njsoncpp decode time testing" << std::endl;
    for(int i = 0; i < TEST_COUNT; i++)
    {
        JsoncppDecodeNew(jsonStrNew);
    }
    nowTime = getNowTime();
    std::cout << "jsoncpp decode " << TEST_COUNT << " time, need time: "
         << nowTime-startTime << "ms"  << std::endl;

    startTime = getNowTime();
    std::cout << "\njsoncpp decode time testing" << std::endl;
    for(int i = 0; i < TEST_COUNT; i++)
    {
        JsoncppDecodeOld(jsonStrNew);
    }
    nowTime = getNowTime();
    std::cout << "jsoncpp decode " << TEST_COUNT << " time, need time: "
         << nowTime-startTime << "ms"  << std::endl;
#endif

    return 0;
}
