#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "cJSON.h"
#include <string.h>


//评论
typedef struct COMMENT
{
    int user_id;
    int msg_id;
    char* comment;
    int click_count;
    int like_count;
    int reply_count;
    int is_new_reply;
    long add_time;
} COMMENT;

//例如
/**
json对象
{
    "user_id":1,
    "msg_id":1,
    "comment":"json",
    "click_count":0,
    "like_count":0,
    "reply_count":0,
    "is_new_reply":0,
    "add_time":0,
}
*/


//获取当前时间毫秒数
long getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    //毫秒
    long time_sec = tv.tv_sec*1000 + tv.tv_usec/1000;

    return time_sec;
}

//打印CJSON
void printfJson(cJSON *json) {
    if (NULL == json) {
        return;
    }
    char *cjson=cJSON_Print(json);//深拷贝
    printf("json:%s\n", cjson);
    free(cjson); //因为调用了cJSON_Print(), 因此需要手动释放资源
}

void freeComment(COMMENT *comment) {
    if (comment != NULL) {
        if (comment->comment != NULL) {
            free(comment->comment);
            comment->comment = NULL;
        }

        free(comment);
        comment = NULL;
    }
}

void freeJson(cJSON *json) {
    if (json != NULL) {
        cJSON_Delete(json);
	}
}

//将结构体转成cJSON
cJSON *convertToJson(COMMENT *comment) 
{
    if (comment == NULL)
        return NULL;

    //创建json
    cJSON *json=cJSON_CreateObject();
    
	//将 "user_id" 添加到 "json" 节点
	cJSON_AddNumberToObject(json, "user_id", comment->user_id);
	cJSON_AddNumberToObject(json, "msg_id", comment->msg_id);
    cJSON_AddItemToObject(json, "comment", cJSON_CreateString(comment->comment));//调用cJSON_CreateString()将字符串转换为JSON中的字符串类型,否则报错
    cJSON_AddNumberToObject(json, "click_count", comment->click_count);
    cJSON_AddNumberToObject(json, "like_count", comment->like_count);
    cJSON_AddNumberToObject(json, "reply_count", comment->reply_count);
    cJSON_AddNumberToObject(json, "is_new_reply", comment->is_new_reply);
    cJSON_AddNumberToObject(json, "add_time", comment->add_time);
	//user_id等的value都是int类型的，因此使用cJSON_AddNumberToObject. 而comment是字符串类型的,因此使用cJSON_AddItemToObject

    return json;
}

//解析cJSON对象, 将cJSON赋值给结构体
COMMENT* convertToComment(cJSON *json)
{  
    if (NULL == json) {
        return NULL;
    }
   
    //通cJSON对象中获取内容
    cJSON *json_user_id = cJSON_GetObjectItem(json, "user_id");
    cJSON *json_msg_id = cJSON_GetObjectItem(json, "msg_id");
    cJSON *json_comment = cJSON_GetObjectItem(json, "comment");
    cJSON *json_click_count = cJSON_GetObjectItem(json, "click_count");
    cJSON *json_like_count = cJSON_GetObjectItem(json, "like_count");
    cJSON *json_reply_count = cJSON_GetObjectItem(json, "reply_count");
    cJSON *json_is_new_reply = cJSON_GetObjectItem(json, "is_new_reply");
    cJSON *json_add_time = cJSON_GetObjectItem(json, "add_time");

	//将内容赋值给结构体
    COMMENT *comment = (COMMENT *)malloc(sizeof(COMMENT));
    comment->user_id = json_user_id->valueint;             //user_id等是使用int来存储的, 因此使用cJSON对象的valueint成员来获取(下同)
    comment->msg_id = json_msg_id->valueint;
    comment->comment=(char *)malloc(256);
    memset(comment->comment, 0, 256);
    strcat(comment->comment, json_comment->valuestring);   //comment是使用字符串来存储的, 因此使用cJSON对象的valuestring成员来获取(下同)
    comment->click_count = json_click_count->valueint;
    comment->like_count = json_like_count->valueint;
    comment->reply_count = json_reply_count->valueint;
    comment->is_new_reply = json_is_new_reply->valueint;
    comment->add_time = (long)(json_add_time->valuedouble); //C中使用long来存储时间, 在cJSON中使用double来存储

    return comment;
}

//从数据中读取数据
void readJsonFromData(char *data) {
    cJSON *json=cJSON_Parse(data);//将data数据构建成cJSON结构
    //打印json
    printfJson(json);

    COMMENT* comment = convertToComment(json);
    if (comment != NULL) {
        
        comment->like_count = 77;
        comment->reply_count = 99;

        //转为json
        cJSON *childjson = convertToJson(comment);
        //打印json
        printfJson(childjson);
        //释放对象
        freeComment(comment);
        //
        freeJson(childjson);
	}
	
	//cJSON_Parse()返回的cJSON对象需要手动释放资源
    freeJson(json);
}

//保存JSON
void writeJsonFile(cJSON *json) {
    if (NULL == json) {
        return;
    }

    char *cjson=cJSON_Print(json);

    FILE *fp = NULL;
    char *fileName = "./comment.json";
    fp = fopen(fileName, "w+");
    if (NULL != fp) {
        fwrite(cjson, strlen(cjson),1,fp);
        fclose(fp);
	}
    if (cjson != NULL) {
        free(cjson);
	}
}

//读取JSON
void readJsonFile() {
    FILE *fp = NULL;
    char *out;
    char line[1024] = {0};
    char *fileName = "./comment.json";

    //打开一个文本文件，文件必须存在，只允许读
    fp = fopen(fileName, "r");
    //seek末尾
    fseek(fp, 0, SEEK_END);
    //读取文件大小
    long len = ftell(fp);
    //seek起始位值
    fseek(fp, 0, SEEK_SET);
    char *data = (char*)malloc(len + 1);
    fread(data, 1, len, fp);
    if (fp != NULL) {
        fclose(fp);
	}

    //读取
    readJsonFromData(data);
    
    if (NULL != data) {
        free(data);
    }
}

COMMENT *createComment() {
    COMMENT *comment = (COMMENT *)malloc(sizeof(COMMENT));
    comment->user_id = 1;
    comment->msg_id = 10000;
    comment->comment=(char *)malloc(256);
    memset(comment->comment, 0, 256);
    sprintf(comment->comment, "我赞同楼主");
    comment->click_count = 0;
    comment->like_count = 0;
    comment->reply_count = 0;
    comment->is_new_reply = 0;
    comment->add_time = getCurrentTime();
    return comment;
}

void test1() {
    COMMENT *comment = createComment();

    //转为json
    cJSON *json = convertToJson(comment);
    
    //保存json
    writeJsonFile(json);

    freeComment(comment);

    freeJson(json);
}

void test2() {
    //读取json
    readJsonFile();
}

int main(int argc, char *argv[]) {
    
    test1();

    test2();

    return 0;
}