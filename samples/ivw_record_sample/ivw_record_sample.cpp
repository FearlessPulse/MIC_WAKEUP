#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "speech_recognizer.h"

#include "aikit_biz_api.h"
#include "aikit_constant.h"
#include "aikit_biz_config.h"

using namespace std;
using namespace AIKIT;
static const char *ABILITY = "e867a88f2";


void OnOutput(AIKIT_HANDLE* handle, const AIKIT_OutputData* output){
    printf("OnOutput abilityID :%s\n",handle->abilityID);
    printf("OnOutput key:%s\n",output->node->key);
    printf("OnOutput value:%s\n",(char*)output->node->value);

    // 播放提示音的命令，这里使用 mpg123 播放 MP3 文件，您可以根据您的环境和需求修改命令
    const char* playCommand = "mpg123 ./resource/start.mp3";
    int commandResult = system(playCommand);

    if (commandResult == 0) {
        printf("提示音播放成功\n");
    } else {
        printf("提示音播放失败\n");
    }
 
}
void OnEvent(AIKIT_HANDLE* handle, AIKIT_EVENT eventType, const AIKIT_OutputEvent* eventValue){
    printf("OnEvent:%d\n",eventType);
}

void OnError(AIKIT_HANDLE* handle, int32_t err, const char* desc){
    printf("OnError:%d\n",err);
}

/* demo recognize the audio from microphone */
static void demo_mic(int keywordfiles_count)
{
    printf("record start!\n");
	int errcode;

	struct speech_rec ivw;

	errcode = sr_init(&ivw, keywordfiles_count, SR_MIC);
	if (errcode) {
		printf("speech recognizer init failed\n");
		return;
	}
	errcode = sr_start_listening(&ivw);
	if (errcode) {
		printf("start listen failed %d\n", errcode);
	}
	char end_command;
    system("stty -icanon");
    while(1)
    {
        end_command = getchar();
        if(end_command == 's')
            break;
        sleep(1);
    }
	errcode = sr_stop_listening(&ivw);
	if (errcode) {
		printf("stop listening failed %d\n", errcode);
	}

	sr_uninit(&ivw);
}

void TestIvw70()
{
    int count = 1;
    int aud_src = 0;

    int ret = AIKIT_Init();
    if(ret != 0){
        printf("AIKIT_Init failed:%d\n",ret);
        goto exit;
    }

    ret = AIKIT_EngineInit(ABILITY,nullptr);
    if(ret != 0){
        printf("AIKIT_EngineInit failed:%d\n",ret);
        goto exit;
    }

    AIKIT_CustomData customData;
    customData.key = "key_word";
    customData.index = 0;
    customData.from = AIKIT_DATA_PTR_PATH;
    customData.value = (void *)"./resource/keyword-ykl.txt";
    customData.len = strlen("./resource/keyword-ykl.txt");
    customData.next = nullptr;
    customData.reserved = nullptr;
    printf("AIKIT_LoadData start!\n");
    ret = AIKIT_LoadData(ABILITY, &customData);
    printf("AIKIT_LoadData end!\n");
    printf("AIKIT_LoadData:%d\n", ret);
    if (ret != 0)
    {
        goto exit;
    }
    demo_mic(count);
 
exit:
    AIKIT_UnLoadData(ABILITY,"key_word",0);

    AIKIT_EngineUnInit(ABILITY);
    AIKIT_UnInit();
}

int main(int argc, char* argv[])
{
	AIKIT_Configurator::builder()
        .app()
            .appID("61fe9a79")
            .apiSecret("MjMyNTBjODg0OWI5ZWI4YTEwYjJkODhk")
            .apiKey("a07d4b9774d67e463e16e04af02835c0")
            .workDir("./")
        .auth()
            .authType(0)
            .ability(ABILITY)
        .log()
            .logLevel(LOG_LVL_INFO)
            .logMode(LOG_FILE)
            .logPath("./aikit.log");

    AIKIT_Callbacks cbs = {OnOutput,OnEvent,OnError};
    AIKIT_RegisterAbilityCallback(ABILITY,cbs);

	TestIvw70();

	return 0;
}
