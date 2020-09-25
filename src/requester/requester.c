#include <curl/curl.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "../../include/cJSON.h"
#include "../../include/requester.h"


#define SR_SINA_URL "http://stock2.finance.sina.com.cn/futures/api/json.php/IndexService.getInnerFuturesMiniKLine%dm?symbol=%s"
//https://hq.sinajs.cn/?_=1588060950614/&list=nf_SR2009    SR2009 区别

CURL* easyhandle;

char result[100000];


void requester_init(){
    //初始化 liburl
	easyhandle = curl_easy_init();
}

void requester_free(){
    free(easyhandle);
}

static int shift ;
static size_t WriteData(void *buffer, size_t size, size_t nmemb,char*str){
  	int res_size;
	res_size = size * nmemb;
	// res_buf = realloc(res_buf, shift+res_size + 1);
	memcpy(result + shift, buffer, res_size);
	shift += res_size;
	return size * nmemb;

}

int requester_requestOne(int minute, char* name){
	shift = 0;
	char* baseUrl = malloc((strlen(SR_SINA_URL) + 10 )*sizeof(char));
	sprintf(baseUrl,SR_SINA_URL, minute, name);
	memset(result, '\0', strlen(result));
	//char * data = "name=danie&project=curl";
	//curl_easy_setopt(easyhandle, CURLOPT_POSTFIELDS, data);
	curl_easy_setopt(easyhandle, CURLOPT_URL,baseUrl); 
	curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, WriteData);
	//curl_easy_setopt(easyhandle, CURLOPT_CHUNK_BGN_FUNCTION, fp);
	curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, &result);
	int ret =	curl_easy_perform(easyhandle);
	free(baseUrl);

    if(ret == CURLE_OK){

        ret = ParseResult(name, result);
    }
	return ret;
}


int ParseResult(char *name, char* result){

	//	printf("request data :%s \n ",resStr);
  cJSON* json = cJSON_Parse(result);
	char timeInday[9];
	int ret = -1;
	if(json){
		//printf("%s \n",cJSON_Print(json));
		//printf("type %d \n",json->type);
		if(json -> type == cJSON_Array && cJSON_GetArraySize(json)>0){
			//数据都是array类型的
			//printf("array size = %d \n",cJSON_GetArraySize(json));
				cJSON *stub = cJSON_GetArrayItem(json, 0);
				if( stub && stub -> type == cJSON_Array){

					char *open = cJSON_GetArrayItem(stub, 1)->valuestring;
					char *top = cJSON_GetArrayItem(stub, 2)->valuestring;
					char *low = cJSON_GetArrayItem(stub, 3)->valuestring;
					char *end = cJSON_GetArrayItem(stub, 4)->valuestring;
					char *deal= cJSON_GetArrayItem(stub, 5)->valuestring;

					ret = atoi(end);
				}
		}
	}
	cJSON_Delete(json);
	return ret;
}

//截取时间上的小时分钟
void ParseTimeInDay(char *orign, char* result, int startIndex){
	memset(result, '\0', strlen(result));
	for(int i =0 ; i< strlen(orign); i++){
		if(i >= startIndex){
			result[i - startIndex] = orign[i];
		}
	}	
}
