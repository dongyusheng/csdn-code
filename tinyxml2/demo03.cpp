//demo03.cpp
//https://github.com/dongyusheng/csdn-code/tree/master/tinyxml2
//增删改查演示版本
#include <iostream>
#include "tinyxml2.h"
#include <stdio.h>
#include <stdlib.h>

using namespace tinyxml2;

/*
enum XMLError {
    XML_SUCCESS = 0,
    XML_NO_ATTRIBUTE,
    XML_WRONG_ATTRIBUTE_TYPE,
    XML_ERROR_FILE_NOT_FOUND,
    XML_ERROR_FILE_COULD_NOT_BE_OPENED,
    XML_ERROR_FILE_READ_ERROR,
    XML_ERROR_PARSING_ELEMENT,
    XML_ERROR_PARSING_ATTRIBUTE,
    XML_ERROR_PARSING_TEXT,
    XML_ERROR_PARSING_CDATA,
    XML_ERROR_PARSING_COMMENT,
    XML_ERROR_PARSING_DECLARATION,
    XML_ERROR_PARSING_UNKNOWN,
    XML_ERROR_EMPTY_DOCUMENT,
    XML_ERROR_MISMATCHED_ELEMENT,
    XML_ERROR_PARSING,
    XML_CAN_NOT_CONVERT_TEXT,
    XML_NO_TEXT_NODE,
	XML_ELEMENT_DEPTH_EXCEEDED,

	XML_ERROR_COUNT
};
*/

//创建XML文件
int createXML(const char* xmlPath)
{
    XMLDocument doc;
    if (XML_ERROR_FILE_NOT_FOUND != doc.LoadFile(xmlPath)) {
        std::cout << "file has been existed !" << std::endl;
        return 0;
    }

    //添加声明 <?xml version="1.0" encoding="UTF-8" ?>
    XMLDeclaration *declaration = doc.NewDeclaration();
    doc.InsertFirstChild(declaration);
    XMLElement *root = doc.NewElement("Users");
    doc.InsertEndChild(root);

    XMLElement *userNode = doc.NewElement("User");
    //添加属性
    userNode->SetAttribute("Name", "dongshao");
    userNode->SetAttribute("Password", "pwd");
    root->InsertEndChild(userNode);
    return doc.SaveFile(xmlPath);
}

void loadXML(const char* xmlPath) {
    XMLDocument doc;
    if (doc.LoadFile(xmlPath) != XML_SUCCESS) {
        std::cout << "load xlm file failed" << std::endl;
        return;
    }

    XMLPrinter printer;
	doc.Print( &printer);
	std::cout << printer.CStr();
}

//添加性别，号码，邮箱      再添加一个用户
int addXML1(const char* xmlPath) {
    XMLDocument doc;
    if (doc.LoadFile(xmlPath) != XML_SUCCESS) {
        std::cout << "load xlm file failed" << std::endl;
        return -1;
    }

    XMLElement *root = doc.RootElement();

    XMLElement *userNode = root->FirstChildElement("User");
    XMLElement *gender = doc.NewElement("Gender");
    XMLText* genderText = doc.NewText("man");
    gender->InsertFirstChild(genderText);
    userNode->InsertFirstChild(gender);

    XMLElement *mobile = doc.NewElement("Mobile");
    mobile->InsertFirstChild(doc.NewText("188****3143"));
    userNode->InsertEndChild(mobile);

    XMLElement *email = doc.NewElement("Email");
    email->InsertFirstChild(doc.NewText("111111111@qq.com"));
    userNode->InsertEndChild(email);

    XMLElement *userNode2 = doc.NewElement("User");
    userNode2->SetAttribute("Name", "Tom");
    userNode2->SetAttribute("Password", "pwd2");
    root->InsertEndChild(userNode2);

    XMLElement *mobile2 = doc.NewElement("Mobile");
    mobile2->InsertFirstChild(doc.NewText("188****3143"));
    userNode2->InsertEndChild(mobile2);

    return doc.SaveFile(xmlPath);
}

//在性别后面添加年龄，再添加一个号码
int addXML2(const char* xmlPath)
{
    XMLDocument doc;
    if (doc.LoadFile(xmlPath) != XML_SUCCESS)
    {
        std::cout<<"load xml file failed"<<std::endl;
        return false;
    }
    
    XMLElement* root=doc.RootElement();
    XMLElement* userNode=root->FirstChildElement("User");
    XMLElement* gender = userNode->FirstChildElement("Gender");
    XMLElement* age = doc.NewElement("Age");
    
    age->InsertFirstChild(doc.NewText("18"));
    userNode->InsertAfterChild(gender,age);
    
    XMLElement* mobile = userNode->FirstChildElement("Mobile");
    mobile->SetAttribute("Location","home");
    
    XMLElement* mobile1 = doc.NewElement("Mobile");
    mobile1->SetAttribute("Location","company");
    
    mobile1->InsertFirstChild(doc.NewText("188****3143"));
    userNode->InsertAfterChild(mobile, mobile1);

    return doc.SaveFile(xmlPath);
}

//删除第一个号码，删除第二个号码的属性
int deleteXML(const char* xmlPath) {
    XMLDocument doc;
    if (doc.LoadFile(xmlPath) != XML_SUCCESS) {
        std::cout << "load xlm file failed" << std::endl;
        return -1;
    }

    XMLElement *root = doc.RootElement();
    XMLElement *userNode = root->FirstChildElement("User");
    XMLElement *mobile = userNode->FirstChildElement("Mobile");
    userNode->DeleteChild(mobile);
    XMLElement *mobile2 = userNode->FirstChildElement("Mobile");
    mobile2->DeleteAttribute("Location");
    
    return doc.SaveFile(xmlPath);
}

//将dongshao的年龄改为10000岁，将Tom的号码改为8888结尾
int updateXML(const char* xmlPath) {
    XMLDocument doc;
    if (doc.LoadFile(xmlPath) != XML_SUCCESS) {
        std::cout << "load xlm file failed" << std::endl;
        return -1;
    }

    XMLElement *root = doc.RootElement();
    XMLElement *userNode = root->FirstChildElement("User");

    while (userNode != NULL) {
        if (0 == strncmp("dongshao", (userNode->Attribute("Name")), 11)) {
            userNode->FirstChildElement("Age")->SetText("10000");
            userNode = userNode->NextSiblingElement();
        } else if (0 == strncmp("Tom", (userNode->Attribute("Name")), 11)) {
            userNode->FirstChildElement("Mobile")->SetText("188****8888");
            userNode = userNode->NextSiblingElement();
        } else {
            userNode = userNode->NextSiblingElement();
        }
    }
    
    return doc.SaveFile(xmlPath);
}

//将dongshao的信息打印出来
int selectXML(const char* xmlPath)
{
    XMLDocument doc;
    if(doc.LoadFile(xmlPath)!=XML_SUCCESS)
    {
        std::cout<<"load xml file failed"<<std::endl;
        return false;
    }
    XMLElement* root=doc.RootElement();
    XMLElement* userNode=root->FirstChildElement("User");
    while(userNode != NULL)
    {
        if( 0 == strncmp("dongshao",(userNode->Attribute("Name")),11))
        {
            std::cout << userNode->Attribute("Name") << std::endl;
            std::cout << userNode->Attribute("Password") << std::endl;
            std::cout << userNode->FirstChildElement("Age")->GetText() << std::endl;
            std::cout << userNode->FirstChildElement("Gender")->GetText() << std::endl;
            std::cout << userNode->FirstChildElement("Mobile")->GetText() << std::endl;
            std::cout << userNode->FirstChildElement("Email")->GetText() << std::endl;
            userNode = userNode->NextSiblingElement();
        } else {
            userNode = userNode->NextSiblingElement();
        }
    }
    return 0;
}

int main( int argc, const char ** argv )
{

    char xmlPath[] = "./demo03.xml";

    /*创建*/
    createXML(xmlPath);
    loadXML(xmlPath);

    printf("------------------------------\n");

    /*增*/
    addXML1(xmlPath);
    loadXML(xmlPath);
 
    printf("------------------------------\n");

    addXML2(xmlPath);
    loadXML(xmlPath);

    printf("------------------------------\n");
    
    /*删*/
    deleteXML(xmlPath);
    loadXML(xmlPath);

    printf("------------------------------\n");

    /*改*/
    updateXML(xmlPath);
    loadXML(xmlPath);
    printf("------------------------------\n");

    /*查*/
    selectXML(xmlPath);

    return 0;
}
