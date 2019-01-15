#pragma once

#include <string>
#include <QMutex>
extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
class FQFMedia
{
public:
	//构造函数，清空错误信息数组
	FQFMedia();
	virtual ~FQFMedia();
	//打开视频文件，如果上次已经打开，会先关闭
	//@param url 视频文件路径
	//@return bool 打开结果
	bool openStream(const char *url);
	//关闭视频
	void closeStream();
	//读取视频帧函数
	//@return 读取到的视频帧，返回值需要用户处理
	AVPacket readStream();
	//视频解码
	//@param pkt 视频帧指针
	//@return 解码结果
	bool decodePacket(const AVPacket *pkt);
	//视频转码
	//@param outSpace 输出空间指针，空间应保证足够的大小，宽X高X4（RGBA）
	//@param outWidth 输出视频的宽
	//@param outHeight 输出视频的高
	//@return bool 转码结果
	bool yuvToRgb(char *outSpace, int outWidth, int outHeight);	
	//获取错误信息
	//@return string 当前的错误信息
	std::string getError();
	//比对视频流序号
	//@param streamIndex pkt的视频流序号
	//@return bool 如果streamIndex序号与视频流序号一致返回true，否则返回false
	bool packetIsVideo(int streamIndex);
	//获取视频尺寸，需要在至少执行过一次decodePacket后才能获取
	//@param w,h 尺寸指针
	bool getYuvSize(int *w,int *h);
    //获取read frame时间，只供read frame超时回掉函数调用
    //@return time_t readStream()调用时间，单位S。
    time_t getStartTime();
    //设置RTSP打开方式，默认tcp，可通过此函数更改打开方式，在openStream之前调用才会生效
    //@param state true tcp,false udp
    void setRtspTransport(bool state);
	//获取当前连接状态
	//@return 连接正常true，异常false
	bool getTimeoutState();
	//超时回掉函数使用
	void setTimeoutState(bool state);
	//设置打开流媒体的超时时间，默认10S，需在打开之前设置
	//@param time 超时时间 单位S
	void setOpenInputTimeout(int time);
	//超时回掉函数使用
	int getOpenInputTimeout();
	//设置读取packet的超时时间，默认3S，需在打开之前设置
	//@param time 超时时间 单位S
	void setReadFrameTimeout(int time);
	//超时回掉函数使用
	int getReadFrameTimeout();
	//超时回掉函数使用
	bool isReadFrame();
	bool reconnect();

protected:
	//C++11可直接在类的声明中赋值
	char errorbuf[1024];			//错误信息
	char rtspUrl[1024];				//打开的rtsp地址
	int reconnectionNum = 0;		//重连计数器
	QMutex mutex;					//线程互斥锁
	AVFormatContext *iCtx = NULL;	//输入视频上下文
	AVFrame *yuv = NULL;			//视频yuv帧
	SwsContext *vCtx = NULL;		//视频转换器
	int videoStream = -1;			//视频流序号
	int yuvWidth = 0;				//yuv宽度
	int yuvHeight = 0;				//yuv高度
    bool rtspTransport = true;      //打开视频方式
    time_t startTime;               //read frame 超时计时器
	bool timeoutState = false;		//超时状态，正常为true，超时为false
	int openInputTimeout = 10;		//打开流媒体超时时间
	int readFrameTimeout = 3;		//读取pkt超时时间
	bool bIsReadFrame = false;		//是否在读取pkt
};

