## 自定义修改唤醒词的离线语音唤醒实现方案二（ubuntu 22.04）

[TOC]

### 实现功能（离线唤醒）：

- 指定唤醒词可随时唤醒目标
- 唤醒词可自定义修改
- 本地修改唤醒词且不需要繁琐生成模型和代码

### 实验方案二：使用讯飞开放平台的 AIkit语音唤醒 Linux SDK

#### 1. 讯飞开放平台的语音唤醒（新版）

#### 2. 下载SDK

- 使用的ubuntu 22.04,下载Linux AIKit

- SDK 包下载（我只选择了语音唤醒一个包）


![image-20240308143930284](wakeup/share/image-20240308143930284.png)

#### 3. SDK 修改编译

主函数在samples目录下：

```bash
$ tree samples/
samples/
├── ivw_record_sample
│   ├── build.sh
│   ├── ivw_record_sample.cpp		# 本次使用有文件唤醒和MIC唤醒
│   ├── linuxrec.c
│   ├── Makefile
│   ├── readme.txt
│   └── speech_recognizer.cpp
└── ivw_sample
    ├── build.sh
    ├── ivw_sample.cpp
    └── readme.txt
```

##### 3.1 SDK初始化参数 (ivw_record_sample.cpp)

- appID、apiKey、apiSecret 和 workDir 为必填项

  ![default](wakeup/share/default.jpeg)

  ```c++
  AIKIT_Configurator::builder()
        .app()      
           .appID("")
           .apiKey("")
           .apiSecret("")
           .workDir("./")          //示例为当前路径
          .auth()
           .ability("e867a88f2")
          .log()  
           .logLevel(LOG_LVL_INFO)
           .logPath("./");
   
  int ret = AIKIT_Init();
  if(ret != 0){
      printf("AIKIT_Init failed:%d\n",ret);
      goto exit;
  }
  ```

##### 3.2 编译

- ivw_record_sample

  ```bash
  $ cd samples
  make			# 直接编译
  $ cd ../bin		# bin目录下会生成一个同名可执行文件 ivw_record_sample
  $ export LD_LIBRARY_PATH=../libs:$LD_LIBRARY_PATH   # 环境变量
  $ ./ivw_record_sample	#运行
  ```

  按提示操作就可以运行了，但我觉得选来选去每次都麻烦，作了一些修改

##### 3.3 自定义唤醒词

- 自定义添加/修改唤醒词

  ```bash
  $ cd PATH/bin/resource
  $ vim many-keywords.txt			#直接按照里面格式进行添加,也可以自己创建 .txt 添加唤醒词
  # 例，添加‘你好老许’ ： ‘ 你好老许;nCM:100; ‘
  $ cat many-keywords.txt 		# 也支持添加多个唤醒词
  你好老许;nCM:100;
  ```

#### 4. 代码修改

##### 4.1 特定唤醒词唤醒(当监测到 ‘你好老许’ 时唤醒并播放提示音)

指定唤醒词**好处**，可以使用特定唤醒词执行不同命令；**坏处**，每次添加唤醒词代码需要改动；

```cpp
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

    if(output->node != nullptr && output->node->value != nullptr){
        const char* wakeupKeyword = "你好老许";
        if (strstr((const char*)output->node->value, wakeupKeyword) != nullptr) {
            printf("唤醒成功！执行特定操作\n");

            // 播放提示音的命令，这里使用 mpg123 播放 MP3 文件，您可以根据您的环境和需求修改命令
            const char* playCommand = "mpg123 PATH/start.mp3";
            int commandResult = system(playCommand);

            if (commandResult == 0) {
                printf("提示音播放成功\n");
            } else {
                printf("提示音播放失败\n");
            }
        }
}
void OnEvent(AIKIT_HANDLE* handle, AIKIT_EVENT eventType, const AIKIT_OutputEvent* eventValue){
    printf("OnEvent:%d\n",eventType);
}

void OnError(AIKIT_HANDLE* handle, int32_t err, const char* desc){
    printf("OnError:%d\n",err);
}

/* demo recognize the audio from microphone */
static void demo_mic(int keywordfiles_count){
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

void TestIvw70(){
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
    if (ret != 0){
        goto exit;
    }
    demo_mic(count);
 
exit:
    AIKIT_UnLoadData(ABILITY,"key_word",0);

    AIKIT_EngineUnInit(ABILITY);
    AIKIT_UnInit();
}

int main(int argc, char* argv[]){
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
```

##### 4.2 任意唤醒词唤醒

每次更换或添加唤醒词不需要在再次修改代码；

```cpp
void OnOutput(AIKIT_HANDLE* handle, const AIKIT_OutputData* output){
    printf("OnOutput abilityID :%s\n",handle->abilityID);
    printf("OnOutput key:%s\n",output->node->key);
    printf("OnOutput value:%s\n",(char*)output->node->value);

    // 播放提示音的命令，这里使用 mpg123 播放 MP3 文件，您可以根据您的环境和需求修改命令
    const char* playCommand = "mpg123 /home/asu/PycharmProjects/mic_file/start.mp3";
    int commandResult = system(playCommand);

    if (commandResult == 0) {
        printf("提示音播放成功\n");
    } else {
        printf("提示音播放失败\n");
    }
}
```

#### 5. 编译脚本

为了便于使用，编写一个便于执行的脚本

```bash
$ cd PATH/bin
$ cd ..
$ sudo chmod 777 run.sh
$ cat run.sh
  #!/bin/bash
  cd ./samples/ivw_record_sample/
  make
  rm *.o
  cd ../../bin
  export LD_LIBRARY_PATH=../libs:$LD_LIBRARY_PATH
  ./ivw_record_sample
$ ./run.sh				# 执行此脚本可直接完成编译和运行唤醒功能
```

**注："你好小迪" 是存在固件中无法去除；（后面有时间会进行屏蔽该唤醒词）**
