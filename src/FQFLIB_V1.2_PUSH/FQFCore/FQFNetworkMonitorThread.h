#ifndef _FQFNETWORKMONITORTHREAD_H_
#define _FQFNETWORKMONITORTHREAD_H_

#include <QThread>
#include "FQFMedia.h"
class FQFNetworkMonitorThread :
	public QThread
{
	Q_OBJECT
public:
	//构造函数，保存播放器指针
    FQFNetworkMonitorThread(FQFMedia **m);
	virtual ~FQFNetworkMonitorThread();
    //设置rtsp地址
    void setRtspUrl(const char *url);
    //设置rtmp地址
	void setRtmpUrl(const char *url);
    //设置rtspID
    void setRtspId(int id);
    //开始监听
	void startMonitor();
    //停止监听
	void stopMonitor();
    //获取id
    int getCameraId();

    QString getRushUrl();

protected slots:
    void reConnectSlot(int id);

protected:
	//读取、解码线程
	void run();
	void generatingRandomAddress();
	void updateRandomAddress();
    FQFMedia **media = NULL;	//播放
    char rtspUrl[1024];         //监控地址
    char rtmpUrl[1024];         //推流地址
    int  cameraId = 0;          //CameraID
	bool isExit = false;		//运行状态
	bool isMonitor = true;		//监听
    bool isRec = false;
    bool isFirstRe = true;
};

#endif
