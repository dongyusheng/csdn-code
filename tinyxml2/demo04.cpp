//demo04.cpp
//增删改查初级版本
#include <iostream>
#include "tinyxml2.h"

#define CHECK_TINYXML2_RESULT(ret)                                                                  \
    do                                                                                              \
    {                                                                                               \
        if (ret != tinyxml2::XML_SUCCESS)                                                           \
        {                                                                                           \
            std::cout << __FUNCTION__ << "(" << __LINE__ << ") failed, ret = " << ret << std::endl; \
            return -1;                                                                              \
        }                                                                                           \
    } while (0)

/**
 * @brief: 写相机属性. 产生完了之后将所有节点挂在p下面
 * @param p 某个相机节点
 * @param doc 所有的元素属性以及值均在doc的下面
 * @param ipAddr IP地址
 * @return: 返回说明
 * @retval: 返回值说明
 */
static void WriteOneCameraAttibute(tinyxml2::XMLElement *p, tinyxml2::XMLDocument &doc,
                                   const char *ipAddr)
{
    const char *subnetMask = "123.45.67.89";

    tinyxml2::XMLElement *pDevID = doc.NewElement("DevID");
    tinyxml2::XMLText *pDevIdText = doc.NewText("5");
    pDevID->InsertEndChild(pDevIdText);
    p->InsertEndChild(pDevID);

    tinyxml2::XMLElement *pIpAddr = doc.NewElement("IpAddress");
    tinyxml2::XMLText *pIpAddrText = doc.NewText(ipAddr);
    pIpAddr->InsertEndChild(pIpAddrText);
    p->InsertEndChild(pIpAddr);

    tinyxml2::XMLElement *pSubnetMask = doc.NewElement("SubnetMask");
    tinyxml2::XMLText *pSubnetMaskText = doc.NewText(subnetMask);
    pSubnetMask->InsertEndChild(pSubnetMaskText);
    p->InsertEndChild(pSubnetMask);

    tinyxml2::XMLElement *pExposureAuto = doc.NewElement("ExposureAuto");
    tinyxml2::XMLText *pExposureAutoText = doc.NewText("false");
    pExposureAuto->InsertEndChild(pExposureAutoText);
    p->InsertEndChild(pExposureAuto);

    tinyxml2::XMLElement *pExposureTime = doc.NewElement("ExposureTime");
    tinyxml2::XMLText *pExposureTimeText = doc.NewText("200");
    pExposureTime->InsertEndChild(pExposureTimeText);
    p->InsertEndChild(pExposureTime);

    tinyxml2::XMLElement *pTriggerMode = doc.NewElement("TriggerMode");
    tinyxml2::XMLText *pTriggerModeText = doc.NewText("false");
    pTriggerMode->InsertEndChild(pTriggerModeText);
    p->InsertEndChild(pTriggerMode);
}
/**
 * @brief: 将数据写到某个xml文件中
 * @param path xml文件路径
 * @return: 返回说明
 * @retval: 返回值说明
 */
static int WriteParam(const std::string &path)
{
    tinyxml2::XMLError ret;
    tinyxml2::XMLDocument doc;
    const char *declaration = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    ret = doc.Parse(declaration);
    CHECK_TINYXML2_RESULT(ret);

    tinyxml2::XMLElement *pFirst = doc.NewElement("CameraI");
    pFirst->SetAttribute("type", "home");
    pFirst->SetAttribute("price", 1000);
    pFirst->SetAttribute("wifi", true);
    // 内部插内容
    WriteOneCameraAttibute(pFirst, doc, "1.22.32.82");
    doc.InsertEndChild(pFirst);

    tinyxml2::XMLElement *pSecond = doc.NewElement("CameraII");
    // 内部插内容
    WriteOneCameraAttibute(pSecond, doc, "2.22.32.82");
    doc.InsertEndChild(pSecond);

    tinyxml2::XMLElement *pThree = doc.NewElement("CameraIII");
    // 内部插内容
    WriteOneCameraAttibute(pThree, doc, "3.22.32.82");
    doc.InsertEndChild(pThree);

    ret = doc.SaveFile(path.c_str());

    return 0;
}

/**
 * @brief: 读取每个相机的属性
 * @param p 相机节点指针
 * @return: 返回说明
 * @retval: 返回值说明
 */
static void ReadOneCameraAttribute(const tinyxml2::XMLElement *p)
{
    int devIdContent = p->FirstChildElement("DevID")->IntText();
    const char *ipAddrContent = p->FirstChildElement("IpAddress")->GetText();
    const char *subnetMaskContent = p->FirstChildElement("SubnetMask")->GetText();
    const char *exposureAutoContent = p->FirstChildElement("ExposureAuto")->GetText();
    int64_t exposureTimeContent = p->FirstChildElement("ExposureTime")->Int64Text();
    bool triggerModeContent = p->FirstChildElement("TriggerMode")->BoolText();

    std::cout << "devIdContent(int):\t" << devIdContent << std::endl;
    std::cout << "ipAddrContent:\t" << ipAddrContent << std::endl;
    std::cout << "subnetMaskContent:\t" << subnetMaskContent << std::endl;
    std::cout << "exposureAutoContent:\t" << exposureAutoContent << std::endl;
    std::cout << "exposureTimeContent(int64_t):\t" << exposureTimeContent << std::endl;
    std::cout << "triggerModeContent(bool):\t" << ((triggerModeContent == true) ? "true" : "false") << std::endl;
}

/**
 * @brief: 读取解析某路径的xml文件
 * @param path xml文件路径
 * @return: 返回说明
 * @retval: 返回值说明
 */
static int ReadParam(const std::string &path)
{
    // 导入文件错误, 退出
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError error = doc.LoadFile(path.c_str());
    CHECK_TINYXML2_RESULT(error);

    tinyxml2::XMLElement *pFirst = doc.FirstChildElement("CameraI"); //doc.RootElement();// 等同
    tinyxml2::XMLElement *pSecond = doc.FirstChildElement("CameraII");
    tinyxml2::XMLElement *pThree = doc.FirstChildElement("CameraIII");

    // 分别读取每个相机的各个属性
    ReadOneCameraAttribute(pFirst);
    std::cout << "------------------\n";
    ReadOneCameraAttribute(pSecond);
    std::cout << "------------------\n";
    ReadOneCameraAttribute(pThree);
    std::cout << "------------------\n";

    return 0;
}

/**
 * @brief: 修改相机属性
 * @param p 相机节点指针
 * @return: 返回说明
 * @retval: 返回值说明
 */
static void ModifyOneCamera(tinyxml2::XMLElement *p)
{
    int devId = 4;
    const char *ipAddr = "139.66.38.13";
    const char *subnetMask = "255.0.0.0";
    bool exposureAuto = false;
    int64_t exposureTime = 80;
    bool triggerMode = false;

    p->FirstChildElement("DevID")->SetText(devId);
    p->FirstChildElement("IpAddress")->SetText(ipAddr);
    p->FirstChildElement("SubnetMask")->SetText(subnetMask);
    p->FirstChildElement("ExposureAuto")->SetText(exposureAuto);
    p->FirstChildElement("ExposureTime")->SetText(exposureTime);
    p->FirstChildElement("TriggerMode")->SetText(triggerMode);
}

/**
 * @brief: 修改某xml文件的参数
 * @param path xml文件路径
 * @return: 返回说明
 * @retval: 返回值说明
 */
static void ModifyParam(const std::string &path)
{
    // 导入文件错误, 退出
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError error = doc.LoadFile(path.c_str());
    if (error != tinyxml2::XMLError::XML_SUCCESS)
        return;

    // 三个相机指针
    tinyxml2::XMLElement *pFirst = doc.FirstChildElement("CameraI");
    tinyxml2::XMLElement *pSecond = doc.FirstChildElement("CameraII");
    tinyxml2::XMLElement *pThree = doc.FirstChildElement("CameraIII");

    // 修改
    ModifyOneCamera(pFirst);
    ModifyOneCamera(pSecond);
    ModifyOneCamera(pThree);

    doc.SaveFile(path.c_str());
}

void testCameraXML()
{
    std::string path = "camera_info_1.xml";
    std::cout << "---------------生成一个xml文件------------------" << std::endl;
    WriteParam(path);

    std::cout << "--------------写文件结束，读取生成的xml文件------------------" << std::endl;
    ReadParam(path);

    std::cout << "--------------读文件结束，修改文件开始------------------" << std::endl;
    ModifyParam(path);

    std::cout << "--------------修改文件结束，读取修改的xml文件------------------" << std::endl;
    ReadParam(path);
}

int main(void)
{
    testCameraXML();
    return 0;
}