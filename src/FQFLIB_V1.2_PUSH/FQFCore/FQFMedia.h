#ifndef _FQFMEDIA_H_
#define _FQFMEDIA_H_

#include <mutex>
#include <QString>
using namespace std;

struct AVFormatContext;
struct AVPacket;

class FQFMedia
{
public:
	//构造函数，清空错误信息数组
	FQFMedia();
	virtual ~FQFMedia();
	//打开输入流媒体，如果上次已经打开，会先关闭
	//@param url 流媒体路径
	//@return bool 打开结果
    bool openInStream(const char *url);
    bool openTestStream(const char *url);
	//打开输出流媒体，如果上次已经打开，会先关闭
	//@param url 流媒体路径
	//@return bool 打开结果
    bool openOutStream(const char *url);
	//关闭
	void closeStream();
	//读取packet函数
	//@return 读取到的视频帧，返回值需要用户调用freePacket()销毁
	bool readStream();
    //推流
	bool pushPacket();
	//获取错误信息
	//@return string 当前的错误信息
    QString getError();
    //获取当前连接状态
    //@return 连接正常true，异常false
    bool getTimeoutState();

protected:
    void closeInStream();
    void closeOutStream();
    void closeTestStream();
    mutex *myLock = NULL;				//线程互斥锁
    AVFormatContext *inFmtCtx = NULL;	//输入流上下文
    AVFormatContext *testFmtCtx = NULL;	//输入流上下文
    AVFormatContext *outFmtCtx = NULL;	//输出流上下文
	AVPacket *packet = NULL;			//pkt
	char errorbuf[1024];				//错误信息
	char rtspUrl[1024];					//打开的rtsp地址
	char rtmpUrl[1024];					//打开的rtmp地址
    bool bTimeoutState = false;         //超时状态
    bool bStartFirst = true;
	int readErrorNum = 0;				//读取错误次数
	int pushErrorNum = 0;				//推流失败次数
};

#endif
