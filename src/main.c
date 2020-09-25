#include <fcntl.h>
#include <stab.h>
#include <sys/cdefs.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "../include/wingtk.h"
#include "../include/cJSON.h"

#define wWidth 600
#define wHeigth 450
/**
 * 目标功能：
 * 1.从配置中读取配置
 * 2.配置列表中的类型，实时查询最新期价
 * 3.配置不同期价类型的提示条件
**/

struct Future
{
	/* data */
	char name[8];
	int gt;
	int lt; 
	long tv_sec;
};

struct Config
{
	/* data */
	int heartBeat;
	struct Future* checkFutures;
	int fsize;
};



struct Config* readConfig (){
	char path[80];
	getcwd(path, sizeof(path));
	char* configName = "/config.json";
	char* configPath = malloc((strlen(path) + strlen(configName)));	
	memcpy(configPath, path, sizeof(path));
	sprintf(configPath, "%s%s", &path, configName); 
	printf("config file = %s\n",configPath);
	
	int fd = open(configPath, O_RDONLY);

	if(fd == -1){
		perror("file not validae");
		return NULL;
	}

	char buffer[2048];
	memset(buffer,'\0', sizeof(buffer));
	int size = read(fd,buffer, sizeof(buffer));
	//todo 此处一次性读取完，若要考虑长度问题， 需要多次读取

	/** parse data  **/

	cJSON *json = cJSON_Parse(buffer);
	struct Config* config = NULL;

	if(json){
		config = malloc(sizeof(struct Config));
		config->heartBeat = cJSON_GetObjectItem(json, "heart_beat")->valueint;
		cJSON * ckFutures = cJSON_GetObjectItem(json, "check_future");

		if(ckFutures->type == cJSON_Array){
			int size = cJSON_GetArraySize(ckFutures);
			struct Future *futures = malloc(sizeof(struct Future) * size);
			for(int i = 0 ; i < size; i ++){
				struct Future* fuTmp = futures + i;
				cJSON* item = cJSON_GetArrayItem(ckFutures,i);

				char* tmpName = cJSON_GetObjectItem(item, "name")->valuestring;
				memset(fuTmp->name, '\0',8);
				memcpy(fuTmp->name, tmpName, strlen(tmpName));
				fuTmp->gt = cJSON_GetObjectItem(item, "gt")->valueint;
				fuTmp->lt = cJSON_GetObjectItem(item, "lt")->valueint;
				fuTmp->tv_sec = 0;
			}
			config->checkFutures = futures;
			config->fsize = size;
		}
		printf("cofnig = %s \n" ,config->checkFutures->name);
	}
	return config;
}


/**
 * 启动轮寻的服务
 **/
void poll_service(struct Config * config){

	if(!config){
		return;
	}

	int ret = 1;
	requester_init();

	while (ret)
	{

		struct Future *item;
		for (int i =0; i<config->fsize; i++){
			item = config->checkFutures + i;
			int number = requester_requestOne(5, item->name);
			char warnStr[20];
			struct timeval* tv = malloc(sizeof(struct timeval));
			gettimeofday(tv,NULL);
			
			printf("%s: 当前值是: %d \n", item->name, number);
			if (number <= 0)
			{
				continue;
			}

			if(number > item->gt){
				if( tv->tv_sec - item->tv_sec > 60 * 30){
					sprintf(warnStr,"%s当前是%d超过了%d",item->name,number,item->gt);
					wingtk_showWarn(warnStr);	
					item->tv_sec = tv->tv_sec;
				}
			}
			if(number < item->lt){
				if( tv->tv_sec - item->tv_sec > 60 * 30){
					sprintf(warnStr,"%s当前是%d小于%d",item->name,number,item->lt);
					wingtk_showWarn(warnStr);	
					item->tv_sec = tv->tv_sec;
				}
			}
			item ++;
			sleep(2);
		}
		
		printf("\n");
		sleep(config->heartBeat);
	}
}

int main(int argc, char**argv)
{
	/** 程序启动  读取配置文件 **/
	struct Config* config = readConfig();
	
	wingtk_init(argc, argv);

	poll_service(config);

	
	//TODO 随时读取输入的命令
	// wingtk_init(argc, argv);	
			

	return 0;
}
