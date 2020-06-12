#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <string.h>

/**
{
	"name":	"milo",
	"age":	80,
	"professional": {专业
        "english": 4,
        "putonghua": 2,
        "computer": 4
    },
    "languages":	["C++", "C"],
	"phone":	{
		"number":	"18620823143",
		"type":	"home"
	},
	"courses":	[{
			"name":	"Linux kernel development",
			"price":	"7.7"
		}, {
			"name":	"Linux server development",
			"price":	"8.0"
		}],
	"vip":	true,
	"address":	null
}
*/

//打印CJSON
void printfJson(cJSON *json) {
    if (NULL == json) {
        return;
    }
    char *cjson=cJSON_Print(json);//深拷贝
    printf("json:%s\n", cjson);
    free(cjson);
}

//释放
void freeJson(cJSON *json) {
    if (json != NULL) {
        cJSON_Delete(json);
	}
}

//创建JSON
cJSON * createJson() {
	//创建根节点JSON(最外面大括号的JSON对象)
    cJSON *json_root=cJSON_CreateObject();
	cJSON_AddItemToObject(json_root, "name", cJSON_CreateString("milo"));
	cJSON_AddNumberToObject(json_root, "age", 80);

	//下面是4个内部JSON对象(包含在根JSON内部的)
    cJSON *json_professional=cJSON_CreateObject();
    cJSON_AddItemToObject(json_professional,"english", cJSON_CreateNumber(4));
    cJSON_AddItemToObject(json_professional,"putonghua", cJSON_CreateNumber(2));
    cJSON_AddItemToObject(json_professional,"computer", cJSON_CreateNumber(3));

    cJSON *json_languages=cJSON_CreateArray(); //注意: 这是一个数组
    cJSON_AddItemToArray(json_languages, cJSON_CreateString("C++"));
    cJSON_AddItemToArray(json_languages, cJSON_CreateString("C"));

    cJSON *json_phone=cJSON_CreateObject();
    cJSON_AddItemToObject(json_phone,"number", cJSON_CreateString("18620823143"));
    cJSON_AddItemToObject(json_phone,"type", cJSON_CreateString("home"));

    cJSON *json_courses = cJSON_CreateArray(); //注意: courses也是一个数组, 其内部有两个对象
    cJSON* json_pItem1 = cJSON_CreateObject();
    cJSON_AddItemToObject(json_pItem1, "name", cJSON_CreateString("Linux kernel development"));
    cJSON_AddNumberToObject(json_pItem1, "price", 7.7);
    cJSON_AddItemToArray(json_courses, json_pItem1);

    cJSON* json_pItem2 = cJSON_CreateObject();
    cJSON_AddItemToObject(json_pItem2, "name", cJSON_CreateString("Linux server development"));
    cJSON_AddNumberToObject(json_pItem2, "price", 8.8);
    cJSON_AddItemToArray(json_courses, json_pItem2);


	//将这个几个内部JSON添加到根节点JSON对象中
    cJSON_AddItemToObject(json_root, "professional", json_professional);
    cJSON_AddItemToObject(json_root, "languages", json_languages);
    cJSON_AddItemToObject(json_root, "phone", json_phone);
    cJSON_AddItemToObject(json_root, "courses", json_courses);

    cJSON_AddBoolToObject(json_root, "vip", 1);
    cJSON_AddNullToObject(json_root, "address");

    return json_root;
}

//向已有的cJSON对象中添加数据(此函数以添加到根节点为例)
//如果要添加到根cJSON对象内部的cJSON对象中(例如添加到professional中), 需要解析cJSON对象, 可以参阅下面的analysisJsonObj()函数
//"hobby": ["Basketball", "Football", "badminton"],
void addDataToJson(cJSON *json) {
    if (NULL == json) {
        return;
    }

    cJSON *hobby=cJSON_CreateArray();
    cJSON_AddItemToArray(hobby, cJSON_CreateString("Basketball"));
    cJSON_AddItemToArray(hobby, cJSON_CreateString("Football"));
    cJSON_AddItemToArray(hobby, cJSON_CreateString("badminton"));

    /*
    CJSON_PUBLIC(void) cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item);
    */
    cJSON_AddItemToObject(json, "hobby", hobby);

    printfJson(json);
}

//修改cJSON对象
//例如将 "hobby": ["Basketball", "Football", "badminton"], 修改为"hobby": ["Skating", "dance"],
void updateDataToJson(cJSON *json) {
    if (NULL == json) {
        return;
    }

    cJSON *hobby=cJSON_CreateArray();
    cJSON_AddItemToArray(hobby, cJSON_CreateString("Skating"));
    cJSON_AddItemToArray(hobby, cJSON_CreateString("dance"));

    /*
    CJSON_PUBLIC(void) cJSON_ReplaceItemInArray(cJSON *array, int which, cJSON *newitem);
    CJSON_PUBLIC(void) cJSON_ReplaceItemInObject(cJSON *object,const char *string,cJSON *newitem);
    */
	
	//修改替换
    cJSON_ReplaceItemInObject(json, "hobby", hobby);

    printfJson(json);
}

//删除
//例如删除"hobby": ["Skating", "dance"],
void deleteDataToJson(cJSON *json) {
    if (NULL == json) {
        return;
    }

    /*
    CJSON_PUBLIC(cJSON *) cJSON_DetachItemFromObject(cJSON *object, const char *string);
    */
    cJSON_DetachItemFromObject(json, "hobby");

    printfJson(json);
}

//解析JSON
void analysisJsonObj(cJSON *json) {
    if (NULL == json) {
        return;
    }

	//解析name
    cJSON *json_name = cJSON_GetObjectItem(json, "name");
    printf("root[%s:%s]\n", json_name->string, json_name->valuestring);
	//解析age
    cJSON *json_age = cJSON_GetObjectItem(json, "age");
    printf("root[%s:%d]\n", json_age->string, json_age->valueint);
	
	//解析professional, 通过调用child和next遍历解析
    cJSON *json_professional = cJSON_GetObjectItem(json, "professional");
    cJSON *json_professional_child = json_professional->child;
    while (json_professional_child != NULL) {
        printf("%s[%s:%d]\n", json_professional->string, json_professional_child->string, json_professional_child->valueint);
        json_professional_child = json_professional_child->next;

    }
	
	//解析languages, 注意languages是一个数组
    cJSON *json_languages = cJSON_GetObjectItem(json, "languages");
    if (json_languages) {
        int size = cJSON_GetArraySize(json_languages);
        printf("%s size:%d\n", json_languages->string, size);
        for (int i = 0; i < size; i++) {
            cJSON *json_languages_child =cJSON_GetArrayItem(json_languages, i);
            printf("%s[%d:%s]\n", json_languages->string, i, json_languages_child->valuestring);
        }
    }

	//解析phone. 另一种方式解析, 直接通过名字获取
    cJSON *json_phone = cJSON_GetObjectItem(json, "phone");
    cJSON *json_phone_number = cJSON_GetObjectItem(json_phone,"number");
    cJSON *json_phone_type = cJSON_GetObjectItem(json_phone,"type");
    printf("%s[%s:%s]\n", json_phone->string, json_phone_number->string, json_phone_number->valuestring);
    printf("%s[%s:%s]\n", json_phone->string, json_phone_type->string, json_phone_type->valuestring);

	//解析courses
    cJSON *json_courses = cJSON_GetObjectItem(json, "courses");
    if (json_courses) {
        int size = cJSON_GetArraySize(json_courses);
        printf("%s size:%d\n", json_courses->string, size);
        for(int i=0;i<size;i++) {
            cJSON *json_array = cJSON_GetArrayItem(json_courses, i);
            if (json_array) {
                cJSON *json_course_name = cJSON_GetObjectItem(json_array,"name");
                cJSON *json_course_price = cJSON_GetObjectItem(json_array,"price");
                printf("%s[%s:%.1lf]\n", json_courses->string, json_course_name->string, json_course_name->valuedouble);
                printf("%s[%s:%.1lf]\n", json_courses->string, json_course_price->string, json_course_price->valuedouble);
            }
        }
    }

	//解析vip, vip是布尔值, 有两种方式解析获取
    cJSON *json_vip = cJSON_GetObjectItem(json, "vip");
    /*
    if (json_vip->type == cJSON_True) {
        printf("root[%s:true]\n", json_vip->string);
    } else if (json_vip->type == cJSON_False) {
        printf("root[%s:false]\n", json_vip->string);
    } else {
        printf("root[%s:false]\n", json_vip->string);
    }
    */
    cJSON_bool bvip = cJSON_IsBool(json_vip);
    if (bvip == cJSON_True) {//cJSON_False
        printf("root[%s:true]\n", json_vip->string);
    } else {
        printf("root[%s:false]\n", json_vip->string);
    }

	//解析address
    cJSON *json_address = cJSON_GetObjectItem(json, "address");
    if (json_address->type ==cJSON_NULL) {
        printf("root[%s:null]\n", json_address->string);
    }
}


//获取cJSON对象的类型, 并同时打印值
static void printJsonObjvalue(const cJSON *json) {
    if (NULL == json) {
        printf("NULL object!\n");
        return;
    }

    switch (json->type) {
        case cJSON_False:
            printf("%s: false\n", json->string);
            break;
        case cJSON_True:
            printf("%s: true\n", json->string);
            break;
        case cJSON_NULL:
            printf("%s: cJSON_NULL\n", json->string);
            break;
        case cJSON_Number:
            printf("%s: %d, %f\n", json->string, json->valueint, json->valuedouble);
            break;
        case cJSON_String:
            printf("%s: %s\n", json->string, json->valuestring);
            break;
        case cJSON_Array:
            printf("%s: cJSON_Array\n", json->string);
            break;
        case cJSON_Object:
            printf("%s: cJSON_Object\n", json->string);
            break;
        default:
            printf("unknown type\n");
            break;
    }
}


void analysisJsonPrint(cJSON *json) {
    if (NULL == json) {
        return;
    }

    cJSON *json_name = cJSON_GetObjectItem(json, "name");
    printJsonObjvalue(json_name);

    cJSON *json_age = cJSON_GetObjectItem(json, "age");
    printJsonObjvalue(json_age);

    cJSON *json_professional = cJSON_GetObjectItem(json, "professional");
    cJSON *json_professional_child = json_professional->child;
    while (json_professional_child != NULL) {
        printJsonObjvalue(json_professional_child);
        json_professional_child = json_professional_child->next;
    }

    cJSON *json_languages = cJSON_GetObjectItem(json, "languages");
    if (NULL == json_languages) {
        int size = cJSON_GetArraySize(json_languages);
        printf("%s size:%d\n", json_languages->string, size);
        for (int i = 0; i < size; i++) {
            cJSON *json_languages_child = cJSON_GetArrayItem(json_languages, i);
            printJsonObjvalue(json_languages_child);
        }
    }

    cJSON *json_phone = cJSON_GetObjectItem(json, "phone");
    cJSON *json_phone_number = cJSON_GetObjectItem(json_phone,"number");
    cJSON *json_phone_type = cJSON_GetObjectItem(json_phone,"type");
    printJsonObjvalue(json_phone_number);
    printJsonObjvalue(json_phone_type);

    cJSON *json_courses = cJSON_GetObjectItem(json, "courses");
    if (json_courses) {
        int size = cJSON_GetArraySize(json_courses);
        printf("%s size:%d\n", json_courses->string, size);
        for(int i=0;i<size;i++) {
            cJSON *arrayItem = cJSON_GetArrayItem(json_courses, i);
            if (NULL != arrayItem) {
                cJSON *json_course_name = cJSON_GetObjectItem(arrayItem,"name");
                cJSON *json_course_price = cJSON_GetObjectItem(arrayItem,"price");
                printJsonObjvalue(json_course_name);
                printJsonObjvalue(json_course_price);
            }
        }
    }

    cJSON *json_vip = cJSON_GetObjectItem(json, "vip");
    printJsonObjvalue(json_vip);

    cJSON *json_address = cJSON_GetObjectItem(json, "address");
    printJsonObjvalue(json_address);
}

//读取JSON
cJSON* readJsonFile(char *fileName) {
    if (NULL == fileName) {
        return NULL;
    }

    FILE *fp = NULL;
    cJSON *json = NULL;
    char line[1024] = {0};
    char *data = NULL;

    //打开一个文本文件，文件必须存在，只允许读
    fp = fopen(fileName, "r");
    if (NULL != fp) {
        //seek末尾
        fseek(fp, 0, SEEK_END);
        //读取文件大小
        long len = ftell(fp);
        //seek起始位值
        fseek(fp, 0, SEEK_SET);
        data = (char*)malloc(len + 1);
        fread(data, 1, len, fp);
        fclose(fp);
    }
    
    printf("readJsonFile data:%s\n", data);
    cJSON *json_root = cJSON_Parse(data);
    if (NULL == json_root) {
        printf("cJSON_Parse error:%s\n", cJSON_GetErrorPtr());
    }

    if (NULL != data) {
        free(data);
    }

    return json_root;
}

//保存JSON
void writeJsonFile(char *fileName, cJSON *json) {
    if (NULL == json || NULL == fileName) {
        return;
    }

    char *cjson=cJSON_Print(json);

    FILE *fp = NULL;
    //新建一个文本文件，已存在的文件将内容清空，允许读写
    fp = fopen(fileName, "w+");
    if (NULL != fp) {
        fwrite(cjson, strlen(cjson),1,fp);
        fclose(fp);
	}

    if (NULL != cjson) {
        free(cjson);
	}
}

void test1() {
    cJSON *json_root = createJson();
    //打印
    printfJson(json_root);
    //解析json
    analysisJsonObj(json_root);
    //解析json
    analysisJsonPrint(json_root);
    freeJson(json_root);
}

void test2() {
    char *fileName = "./demo1.json";

    //读取json
    cJSON *json_root = readJsonFile(fileName);
    //解析json
    analysisJsonObj(json_root);
    //解析json
    analysisJsonPrint(json_root);

    //写入json文件
    writeJsonFile(fileName, json_root);

    //释放
    freeJson(json_root);
}

//增删改
void test3() {
    cJSON *json_root = createJson();

    addDataToJson(json_root);

    updateDataToJson(json_root);

    deleteDataToJson(json_root);

    freeJson(json_root);
}

int main(int argc, char *argv[]) {
    
    printf("-----------------------------------\n");
    test1();
    printf("-----------------------------------\n");
    test2();
    printf("-----------------------------------\n");
    test3();
    printf("-----------------------------------\n");

    return 0;
}