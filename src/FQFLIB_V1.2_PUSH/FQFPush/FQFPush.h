#ifndef _FQFPUSH_H_
#define _FQFPUSH_H_

#include "FQFCore/FQFMedia.h"
#include "FQFCore/FQFMediaPush.h"
#include "FQFCore/FQFNetworkMonitorThread.h"

class FQFPush
{
public:
	//构造函数，清空错误信息数组
	FQFPush();
	virtual ~FQFPush();
    //启动一个转码工作
    void startPush(const char *rtspUrl, const char *rtmpUrl,int id);
    //停止
    void stopPush();
    //获取状态
    bool getState();
    //获取实际推流地址
    QString getPushUrl();
    //获取摄像头ID
    int getCameraId();

    bool getTemp();

    void reConnect();

protected:
	FQFMedia *media = NULL;
	FQFMediaPush *push = NULL;
	FQFNetworkMonitorThread *monitor = NULL;
};

#endif
