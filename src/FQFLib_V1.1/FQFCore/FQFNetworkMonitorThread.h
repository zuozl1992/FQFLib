#pragma once

#include <QThread>
#include "FQFMedia.h"
class FQFNetworkMonitorThread :
	public QThread
{
	Q_OBJECT
public:
	FQFNetworkMonitorThread(FQFMedia *m);
	virtual ~FQFNetworkMonitorThread();
	bool getConnect();
	void setRtspUrl(const char *url);
	void startMonitor();
	void stopMonitor();
	void realseMonitor();

protected:
	void run();
	FQFMedia *media = NULL;
	char rtspUrl[1024] = { 0 };
	bool isExit = false;
	bool isMonitor = true;
	bool isConnect = false;
};

