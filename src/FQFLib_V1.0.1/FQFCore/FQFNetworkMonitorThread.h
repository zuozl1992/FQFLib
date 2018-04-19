#pragma once

#include <QThread>
#include "FQFMedia.h"
class FQFNetworkMonitorThread :
	public QThread
{
	Q_OBJECT
public:
	//构造函数，保存播放器指针
	FQFNetworkMonitorThread(FQFMedia *m);
	virtual ~FQFNetworkMonitorThread();
	bool getConnect();
	void setRtspUrl(const char *url);
	void startMonitor();
	void stopMonitor();
	void realseMonitor();

protected:
	//读取、解码线程
	void run();
	FQFMedia *media = NULL;		//播放
	char rtspUrl[1024] = { 0 };	//监控地址
	bool isExit = false;		//运行状态
	bool isMonitor = true;		//监听
	bool isConnect = false;		//连接状态
};

