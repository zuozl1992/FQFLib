#ifndef _FQFMEDIA_H_
#define _FQFMEDIA_H_

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
	//打开流媒体，如果上次已经打开，会先关闭
	//@param url 流媒体路径
	//@return bool 打开结果
	bool openStream(const char *url);
	//关闭
	void closeStream();
	//读取packet函数
	//@return 读取到的视频帧，返回值需要用户调用freePacket()销毁
	AVPacket readStream();
	//销毁packet
	//@param pkt 销毁对象
	void freePacket(AVPacket *pkt);
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
    //设置RTSP打开方式，默认tcp，可通过此函数更改打开方式，在openStream之前调用才会生效
    //@param state true tcp,false udp
    void setRtspTransport(bool state);
	//获取当前连接状态
	//@return 连接正常true，异常false
	bool getTimeoutState();
	//设置打开流媒体的超时时间，默认10S，需在打开之前设置
	//@param time 超时时间 单位S
	void setOpenInputTimeout(int time);
	//设置读取packet的超时时间，默认3S，需在打开之前设置
	//@param time 超时时间 单位S
	void setReadFrameTimeout(int time);

	//超时回掉函数使用，内部调用，逻辑保护函数。
	//获取read frame时间，只供read frame超时回掉函数调用
	//@return time_t readStream()调用时间，单位S。
	time_t getStartTime();
	//超时回掉函数使用 设置当前连接状态
	void setTimeoutState(bool state);
	//超时回掉函数使用 获取触发时间
	int getOpenInputTimeout();
	//超时回掉函数使用 获取触发时间
	int getReadFrameTimeout();
	//超时回掉函数使用 判断超时状态
	bool isReadFrame();

protected:
	QMutex *locker = NULL;				//线程互斥锁
	AVFormatContext *inFmtCtx = NULL;	//输入视频上下文
	AVFrame *videoFrame = NULL;			//视频yuv帧
	SwsContext *videoSwsCtx = NULL;		//视频转换器
	time_t startTime;					//起始时间
	char errorbuf[1024];				//错误信息
	char rtspUrl[1024];					//打开的rtsp地址
	bool bRtspTransport = true;			//打开视频方式
	bool bTimeoutState = false;			//超时状态，正常为true，超时为false
	bool bIsReadFrame = false;			//是否在读取pkt
	int iVideoStream = -1;				//视频流序号
	int iWidth = 0;						//视频宽度
	int iHeight = 0;					//视频高度
	int iOpenInputTimeout = 5;			//打开流媒体超时时间
	int iReadFrameTimeout = 3;			//读取Frame超时时间
	int readErrorNum = 0;				//读取错误次数
	int decodeErrorNum = 0;				//解码错误次数
	int swsErrorNum = 0;				//转码错误次数
};

#endif