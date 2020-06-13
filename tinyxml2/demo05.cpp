//demo05.cpp
//增删改查高级版本
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

typedef struct cameraInfo
{
    int id;
    std::string type;
    std::string ipAddr;
    std::string subnetMask;
    bool exposureAuto;
    int64_t exposureTime;
    bool triggerMode;
    float price;
} CAMERA_INFO_T;

/**
 * @brief: 写入类型1 Camera的测试信息
 * @param p  
 * @param doc  
 * @return: 返回说明
 * @retval: 返回值说明
 */
static void WriteCameraI(tinyxml2::XMLElement *p, tinyxml2::XMLDocument &doc)
{
    const char *ipAddr = "178.26.85.83";
    const char *subnetMask = "123.45.67.89";

    // 1. 创建一个CameraI
    tinyxml2::XMLElement *cameraI_1 = doc.NewElement("CameraI");
    // 2. 封装CameraI
    // 写入属性
    cameraI_1->SetAttribute("type", "home");
    cameraI_1->SetAttribute("id", 5);
    // 写入IP地址
    tinyxml2::XMLElement *pIpAddr = doc.NewElement("IpAddress");
    tinyxml2::XMLText *pIpAddrText = doc.NewText(ipAddr);
    pIpAddr->InsertEndChild(pIpAddrText);
    cameraI_1->InsertEndChild(pIpAddr);
    // 写入子网掩码
    tinyxml2::XMLElement *pSubnetMask = doc.NewElement("SubnetMask");
    tinyxml2::XMLText *pSubnetMaskText = doc.NewText(subnetMask);
    pSubnetMask->InsertEndChild(pSubnetMaskText);
    cameraI_1->InsertEndChild(pSubnetMask);
    // 写入是否自动曝光
    tinyxml2::XMLElement *pExposureAuto = doc.NewElement("ExposureAuto");
    tinyxml2::XMLText *pExposureAutoText = doc.NewText("false");
    pExposureAuto->InsertEndChild(pExposureAutoText);
    cameraI_1->InsertEndChild(pExposureAuto);
    // 写入曝光时长
    tinyxml2::XMLElement *pExposureTime = doc.NewElement("ExposureTime");
    tinyxml2::XMLText *pExposureTimeText = doc.NewText("200");
    pExposureTime->InsertEndChild(pExposureTimeText);
    cameraI_1->InsertEndChild(pExposureTime);
    // 写入触发模式
    tinyxml2::XMLElement *pTriggerMode = doc.NewElement("TriggerMode");
    tinyxml2::XMLText *pTriggerModeText = doc.NewText("false");
    pTriggerMode->InsertEndChild(pTriggerModeText);
    cameraI_1->InsertEndChild(pTriggerMode);
    // 写入价格
    tinyxml2::XMLElement *pPrice = doc.NewElement("Price");
    tinyxml2::XMLText *pPriceText = doc.NewText("9.9");
    pPrice->InsertEndChild(pPriceText);
    cameraI_1->InsertEndChild(pPrice);
    // 3. 将一个CameraI插入到CameraConfig
    p->InsertEndChild(cameraI_1);

    // 1. 创建一个CameraI
    tinyxml2::XMLElement *cameraI_2 = doc.NewElement("CameraI");
    // 2. 封装CameraI
    // 写入属性
    cameraI_2->SetAttribute("type", "company");
    cameraI_2->SetAttribute("id", 6);
    // 写入IP地址
    pIpAddr = doc.NewElement("IpAddress");
    pIpAddrText = doc.NewText(ipAddr);
    pIpAddr->InsertEndChild(pIpAddrText);
    cameraI_2->InsertEndChild(pIpAddr);
    // 写入子网掩码
    pSubnetMask = doc.NewElement("SubnetMask");
    pSubnetMaskText = doc.NewText(subnetMask);
    pSubnetMask->InsertEndChild(pSubnetMaskText);
    cameraI_2->InsertEndChild(pSubnetMask);
    // 写入是否自动曝光
    pExposureAuto = doc.NewElement("ExposureAuto");
    pExposureAutoText = doc.NewText("false");
    pExposureAuto->InsertEndChild(pExposureAutoText);
    cameraI_2->InsertEndChild(pExposureAuto);
    // 写入曝光时长
    pExposureTime = doc.NewElement("ExposureTime");
    pExposureTimeText = doc.NewText("200");
    pExposureTime->InsertEndChild(pExposureTimeText);
    cameraI_2->InsertEndChild(pExposureTime);
    // 写入触发模式
    pTriggerMode = doc.NewElement("TriggerMode");
    pTriggerModeText = doc.NewText("false");
    pTriggerMode->InsertEndChild(pTriggerModeText);
    cameraI_2->InsertEndChild(pTriggerMode);
    // 写入价格
    // 3. 将一个CameraI插入到CameraConfig
    p->InsertEndChild(cameraI_2);
}

/**
 * @brief: 写入类型2 Camera的测试信息
 * @param p  
 * @param doc  
 * @return: 返回说明
 * @retval: 返回值说明
 */
static void WriteCameraII(tinyxml2::XMLElement *p, tinyxml2::XMLDocument &doc)
{
    const char *ipAddr = "2.26.85.83";
    const char *subnetMask = "123.45.67.89";

    // 1. 创建一个CameraII
    tinyxml2::XMLElement *cameraII_1 = doc.NewElement("CameraII");
    // 2. 封装CameraII
    // 写入设备ID
    tinyxml2::XMLElement *pDevID = doc.NewElement("DevID");
    tinyxml2::XMLText *pDevIdText = doc.NewText("7");
    pDevID->InsertEndChild(pDevIdText);
    cameraII_1->InsertEndChild(pDevID);
    // 写入IP地址
    tinyxml2::XMLElement *pIpAddr = doc.NewElement("IpAddress");
    tinyxml2::XMLText *pIpAddrText = doc.NewText(ipAddr);
    pIpAddr->InsertEndChild(pIpAddrText);
    cameraII_1->InsertEndChild(pIpAddr);
    // 写入子网掩码
    tinyxml2::XMLElement *pSubnetMask = doc.NewElement("SubnetMask");
    tinyxml2::XMLText *pSubnetMaskText = doc.NewText(subnetMask);
    pSubnetMask->InsertEndChild(pSubnetMaskText);
    cameraII_1->InsertEndChild(pSubnetMask);
    // 写入是否自动曝光
    tinyxml2::XMLElement *pExposureAuto = doc.NewElement("ExposureAuto");
    tinyxml2::XMLText *pExposureAutoText = doc.NewText("false");
    pExposureAuto->InsertEndChild(pExposureAutoText);
    cameraII_1->InsertEndChild(pExposureAuto);
    // 写入曝光时长
    tinyxml2::XMLElement *pExposureTime = doc.NewElement("ExposureTime");
    tinyxml2::XMLText *pExposureTimeText = doc.NewText("200");
    pExposureTime->InsertEndChild(pExposureTimeText);
    cameraII_1->InsertEndChild(pExposureTime);
    // 写入触发模式
    tinyxml2::XMLElement *pTriggerMode = doc.NewElement("TriggerMode");
    tinyxml2::XMLText *pTriggerModeText = doc.NewText("false");
    pTriggerMode->InsertEndChild(pTriggerModeText);
    cameraII_1->InsertEndChild(pTriggerMode);
    // 写入价格
    tinyxml2::XMLElement *pPrice = doc.NewElement("Price");
    tinyxml2::XMLText *pPriceText = doc.NewText("9.9");
    pPrice->InsertEndChild(pPriceText);
    cameraII_1->InsertEndChild(pPrice);
    // 3. 将一个CameraI插入到CameraConfig
    p->InsertEndChild(cameraII_1);

    // 1. 创建一个CameraII
    tinyxml2::XMLElement *cameraII_2 = doc.NewElement("CameraII");
    // 2. 封装CameraII
    // 写入设备ID
    pDevID = doc.NewElement("DevID");
    pDevIdText = doc.NewText("8");
    pDevID->InsertEndChild(pDevIdText);
    cameraII_2->InsertEndChild(pDevID);
    // 写入IP地址
    pIpAddr = doc.NewElement("IpAddress");
    pIpAddrText = doc.NewText(ipAddr);
    pIpAddr->InsertEndChild(pIpAddrText);
    cameraII_2->InsertEndChild(pIpAddr);
    // 写入子网掩码
    pSubnetMask = doc.NewElement("SubnetMask");
    pSubnetMaskText = doc.NewText(subnetMask);
    pSubnetMask->InsertEndChild(pSubnetMaskText);
    cameraII_2->InsertEndChild(pSubnetMask);
    // 写入是否自动曝光
    pExposureAuto = doc.NewElement("ExposureAuto");
    pExposureAutoText = doc.NewText("false");
    pExposureAuto->InsertEndChild(pExposureAutoText);
    cameraII_2->InsertEndChild(pExposureAuto);
    // 写入曝光时长
    pExposureTime = doc.NewElement("ExposureTime");
    pExposureTimeText = doc.NewText("400");
    pExposureTime->InsertEndChild(pExposureTimeText);
    cameraII_2->InsertEndChild(pExposureTime);
    // 写入触发模式
    pTriggerMode = doc.NewElement("TriggerMode");
    pTriggerModeText = doc.NewText("false");
    pTriggerMode->InsertEndChild(pTriggerModeText);
    cameraII_2->InsertEndChild(pTriggerMode);
    // 写入价格
    // 3. 将一个CameraI插入到CameraConfig
    p->InsertEndChild(cameraII_2);
}
/**
 * @brief: 写入类型Camera的测试信息
 * @param path   路径
 * @return: 返回说明
 * @retval: 返回值说明
 */
static int WriteXML(const std::string &path)
{
    tinyxml2::XMLError ret;
    tinyxml2::XMLDocument doc;
    const char *declaration = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    ret = doc.Parse(declaration);
    CHECK_TINYXML2_RESULT(ret);

    tinyxml2::XMLElement *root = doc.NewElement("CameraConfig");
    root->SetAttribute("version", "1.0");
    WriteCameraI(root, doc);
    WriteCameraII(root, doc);
    doc.InsertEndChild(root); // InsertEndChild 添加节点

    ret = doc.SaveFile(path.c_str());
    CHECK_TINYXML2_RESULT(ret);

    return 0;
}

/**
 * @brief: 读取一个camera信息
 * @param {type} 形参 参数说明
 * @return: 返回说明
 * @retval: 返回值说明
 */
static void ReadOneCameraXML(const tinyxml2::XMLElement *p)
{
    std::cout << "Model:" << p->Name() << std::endl;

    const tinyxml2::XMLAttribute *type = p->FindAttribute("type");
    if (type)
    {
        std::cout << "camera type:\t" << type->Value() << std::endl;
    }
    const tinyxml2::XMLAttribute *id = p->FindAttribute("id");
    if (id)
    {
        std::cout << "camera id:\t" << id->IntValue() << std::endl;
    }

    const tinyxml2::XMLElement *devId = p->FirstChildElement("DevID");
    if (devId) // 判断某个元素是否存在
    {
        int devIdContent = devId->IntText();
        std::cout << "devIdContent(int):\t" << devIdContent << std::endl;
    }

    const char *ipAddrContent = p->FirstChildElement("IpAddress")->GetText();
    const char *subnetMaskContent = p->FirstChildElement("SubnetMask")->GetText();
    const char *exposureAutoContent = p->FirstChildElement("ExposureAuto")->GetText();
    int64_t exposureTimeContent = p->FirstChildElement("ExposureTime")->Int64Text();
    bool triggerModeContent = p->FirstChildElement("TriggerMode")->BoolText();

    std::cout << "ipAddrContent:\t" << ipAddrContent << std::endl;
    std::cout << "subnetMaskContent:\t" << subnetMaskContent << std::endl;
    std::cout << "exposureAutoContent:\t" << exposureAutoContent << std::endl;
    std::cout << "exposureTimeContent(int64_t):\t" << exposureTimeContent << std::endl;
    std::cout << "triggerModeContent(bool):\t" << ((triggerModeContent == true) ? "true" : "false") << std::endl;
    const tinyxml2::XMLElement *pPrice = p->FirstChildElement("Price");
    if (pPrice) // 判断某个元素是否存在
    {
        std::cout << "price(float):\t" << pPrice->FloatText() << std::endl;
    }
}
/**
 * @brief: 遍历XML
 * @param path 本地文件路径
 * @return: 返回说明
 * @retval: 返回值说明
 */
static int ParseXML(const std::string &path)
{
    std::cout << "ParseXML into" << std::endl;
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError error = doc.LoadFile(path.c_str());
    CHECK_TINYXML2_RESULT(error); // 如果错误则退出
    std::cout << "ParseXML:" << std::endl;

    tinyxml2::XMLElement *root = doc.RootElement();
    tinyxml2::XMLElement *element = NULL;
    element = root->FirstChildElement();

    while (true)
    {
        if (element)
        {
            std::cout << "-------------------------------------" << std::endl;
            ReadOneCameraXML(element);
        }
        else
        {
            std::cout << "-------------------------------------" << std::endl;
            std::cout << "ParseXML finish" << std::endl;
            break;
        }
        element = element->NextSiblingElement();
    }

    return 0;
}

/**
 * @brief: 修改一个CameraI
 * @param {type} 形参 参数说明
 * @return: 返回说明
 * @retval: 返回值说明
 */
static void ModifyOneCameraById(tinyxml2::XMLElement *root, tinyxml2::XMLDocument &doc, const CAMERA_INFO_T &cameraInfo)
{
    tinyxml2::XMLElement *element = NULL;
    element = root->FirstChildElement();
    bool isFindId = false;
    while (true)
    {
        if (element)
        {
            const tinyxml2::XMLAttribute *id = element->FindAttribute("id");
            if (id)
            {
                std::cout << "current id:\t" << id->IntValue() << std::endl;
                if (id->IntValue() == cameraInfo.id)
                {
                    std::cout << "Find id:\t" << id->IntValue() << std::endl;
                    isFindId = true;
                    break;
                }
            }

            const tinyxml2::XMLElement *devId = element->FirstChildElement("DevID");
            if (devId) // 判断某个元素是否存在
            {
                std::cout << "current id:\t" << id->IntValue() << std::endl;
                if (id->IntValue() == cameraInfo.id)
                {
                    std::cout << "Find id:\t" << id->IntValue() << std::endl;
                    isFindId = true;
                    break;
                }
            }
        }
        else
        {
            std::cout << "-------------------------------------" << std::endl;
            std::cout << "Find XML finish, not find id:\t" << cameraInfo.id << std::endl;
            break;
        }
        // element = root->NextSiblingElement();
        std::cout << "----------NextSiblingElement------------" << std::endl;
        element = element->NextSiblingElement(); // 兄弟节点
    }
    if (isFindId)
    {
        std::cout << "isFindId: " << isFindId << std::endl;
        std::cout << "ipAddr: " << cameraInfo.ipAddr.c_str() << std::endl;
        element->FirstChildElement("IpAddress")->SetText(cameraInfo.ipAddr.c_str());
        element->FirstChildElement("SubnetMask")->SetText(cameraInfo.subnetMask.c_str());
        element->FirstChildElement("ExposureAuto")->SetText(cameraInfo.exposureAuto);
        element->FirstChildElement("ExposureTime")->SetText(cameraInfo.exposureTime);
        element->FirstChildElement("TriggerMode")->SetText(cameraInfo.triggerMode);

        element->SetAttribute("type", cameraInfo.type.c_str());
        tinyxml2::XMLElement *pPrice = element->FirstChildElement("Price");
        if (pPrice)
        {
            pPrice->SetText(cameraInfo.price);
        }
        else // 查找失败则说明没有该属性，需要添加
        {
            pPrice = doc.NewElement("Price");
            char cpriceBuf[20];
            sprintf(cpriceBuf, "%0.2f", cameraInfo.price);
            std::cout << " Add price: " << cpriceBuf << std::endl;
            tinyxml2::XMLText *pPriceText = doc.NewText(cpriceBuf);
            tinyxml2::XMLComment *pPriceComment = doc.NewComment("add price");
            element->InsertEndChild(pPriceComment);
            pPrice->InsertEndChild(pPriceText);
            element->InsertEndChild(pPrice);
        }
    }
}

/**
 * @brief: 修改指定ID的camera的参数
 * @param path 路径
 * @param camearInfo 新的参数信息
 * @return: 返回说明
 * @retval: 返回值说明
 */
static int ModifyXMLById(const std::string &path, const CAMERA_INFO_T &camearInfo)
{
    // 导入文件错误, 退出
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError error = doc.LoadFile(path.c_str());
    CHECK_TINYXML2_RESULT(error);

    tinyxml2::XMLElement *root = doc.FirstChildElement("CameraConfig");
    ModifyOneCameraById(root, doc, camearInfo);

    error = doc.SaveFile(path.c_str());
    CHECK_TINYXML2_RESULT(error);

    return 0;
}

void testCameraXML()
{
    std::string path = "camera_config_2.xml";
    // // std::string path = "camera_info.xml";
    std::cout << "---------------生成一个xml文件------------------" << std::endl;
    WriteXML(path);

    std::cout << "--------------写文件结束，读取生成的xml文件------------------" << std::endl;
    ParseXML(path);

    std::cout << "--------------读文件结束，根据ID查找camera并修改--------------" << std::endl;
    CAMERA_INFO_T cameraInfo;
    cameraInfo.id = 6;
    cameraInfo.type = "hotel";
    cameraInfo.ipAddr = "111.26.85.8";
    cameraInfo.subnetMask = "111.26.85.255";
    cameraInfo.exposureAuto = true;
    cameraInfo.triggerMode = false;
    cameraInfo.price = 100;
    ModifyXMLById(path, cameraInfo);

    std::cout << "--------------修改文件结束，读取修改的xml文件------------------" << std::endl;
    ParseXML(path);
}
int main(void)
{
    testCameraXML();
    return 0;
}