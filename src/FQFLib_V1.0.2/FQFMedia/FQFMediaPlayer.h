#pragma once

#include <QThread>
#include "FQF/FQFMedia/FQFMedia.h"
class FQFMediaPlayer :
	public QThread
{
public:
	//构造函数，保存播放器指针
	FQFMediaPlayer();
	virtual ~FQFMediaPlayer();
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
	bool getYuvSize(int *w, int *h);
	//设置播放状态
	//@parma state true 解码 false 停止
	void setStreamState(bool state);
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
    //读取、解码线程
	void run();
protected:
	FQFMedia *media = NULL;		//播放
	bool isExit = true;			//运行状态
	bool isPlay = true;			//播放状态
};

