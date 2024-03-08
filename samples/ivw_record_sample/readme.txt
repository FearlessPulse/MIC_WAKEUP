################################################################################################
该sample是包含录音唤醒和文件唤醒两种方式，供用户选择使用
################################################################################################

1、进入sample/ivw_record_sample目录，修改ivw_record_sample.cpp，将appid、appKey、appSecret写入demo；
2、执行./build.sh进行编译
3、cd ../../bin/ 进入到bin目录下，设置环境变量：# export LD_LIBRARY_PATH=../libs:$LD_LIBRARY_PATH
4、在bin目录下，获取目录下所有文件名：find ./resource/cn_wav/ -type f > wav.scp
5、执行编译后的可执行文件 # ./ivw_record_sample