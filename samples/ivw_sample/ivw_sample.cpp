/*
 * @Description: 
 * @version: 
 * @Author: jhfan5
 * @Date: 2021-09-16 17:32:41
 * @LastEditors: jhfan5
 * @LastEditTime: 2021-09-16 17:50:30
 */

#include <fstream>
#include <assert.h>
#include <cstring>
#include <atomic>
#include <unistd.h>

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
}
void OnEvent(AIKIT_HANDLE* handle, AIKIT_EVENT eventType, const AIKIT_OutputEvent* eventValue){
    printf("OnEvent:%d\n",eventType);
}

void OnError(AIKIT_HANDLE* handle, int32_t err, const char* desc){
    printf("OnError:%d\n",err);
}

void ivwIns(const char* audio_path,int keywordfiles_count){
    AIKIT_ParamBuilder* paramBuilder = nullptr;
    AIKIT_DataBuilder* dataBuilder = nullptr;
    AIKIT_HANDLE* handle = nullptr;
    AiAudio* aiAudio_raw = nullptr;
    int fileSize = 0;
    int readLen = 0;
    FILE * file = nullptr;
    char data[320] = {0};
    int *index = NULL;
    int writetimes = 0;
    int ret = 0;

    paramBuilder = AIKIT_ParamBuilder::create();
    index = (int *)malloc(keywordfiles_count * sizeof(int));
    for (int i = 0; i < keywordfiles_count;++i)
        index[i] = i;
    ret = AIKIT_SpecifyDataSet(ABILITY, "key_word", index, keywordfiles_count);
    printf("AIKIT_SpecifyDataSet:%d\n", ret);
    if(ret != 0){
        goto  exit;
    }
    paramBuilder->param("wdec_param_nCmThreshold","0 0:1000",strlen("0 0:1000"));
    paramBuilder->param("gramLoad",true);
    ret = AIKIT_Start(ABILITY,AIKIT_Builder::build(paramBuilder),nullptr,&handle);
    printf("AIKIT_Start:%d\n",ret);
    if(ret != 0){
        goto  exit;
    }

    file = fopen(audio_path,"rb");
    if(file == nullptr) {
        printf("fopen failed\n");
        goto exit;
    }
    fseek(file,0,SEEK_END);
    fileSize = ftell(file);
    fseek(file,0,SEEK_SET);

    dataBuilder = AIKIT_DataBuilder::create();
    while (fileSize > 0) {
        readLen = fread(data,1,sizeof(data),file);
        dataBuilder->clear();

        aiAudio_raw = AiAudio::get("wav")->data(data,320)->valid();
        dataBuilder->payload(aiAudio_raw);
        ret = AIKIT_Write(handle,AIKIT_Builder::build(dataBuilder));
        writetimes++;
        if(ret != 0){
            printf("AIKIT_Write:%d\n",ret);
            goto  exit;
        }
       fileSize -= readLen;
    }
    ret = AIKIT_End(handle);

 exit:   
    if(index != NULL)
        free(index);
    if(file != nullptr){
        fclose(file);
        file = nullptr;
    }
    if(handle != nullptr)
        AIKIT_End(handle);

    if(paramBuilder != nullptr){
        delete paramBuilder;
        paramBuilder = nullptr;
    }

    if(dataBuilder != nullptr){
        delete dataBuilder;
        dataBuilder = nullptr;
    }
}

void TestIvw70(){
    int times = 1;

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

    if (times == 1){
        AIKIT_CustomData customData;
        customData.key = "key_word";
        customData.index = 0;
        customData.from = AIKIT_DATA_PTR_PATH;
        customData.value =(void*)"./resource/keyword-fynh.txt";
        customData.len = strlen("./resource/keyword-fynh.txt");
        customData.next = nullptr;
        customData.reserved = nullptr;
        printf("AIKIT_LoadData start!\n");
        ret = AIKIT_LoadData(ABILITY,&customData);
        printf("AIKIT_LoadData end!\n");
        printf("AIKIT_LoadData:%d\n",ret);
        if(ret != 0){
            goto  exit;
        }
        times++;
    }

    ivwIns("./audio/fynh.wav", 1);
exit:   
    AIKIT_UnLoadData(ABILITY,"key_word",0);

    AIKIT_EngineUnInit(ABILITY);
    AIKIT_UnInit();
}



int main() {
    
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
            .logMode(LOG_STDOUT)
            .logPath("./aikit.log");

    AIKIT_Callbacks cbs = {OnOutput,OnEvent,OnError};
    AIKIT_RegisterAbilityCallback(ABILITY,cbs);

    TestIvw70();

    return 0;
}
