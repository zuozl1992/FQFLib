#include "FQFNetworkMonitorThread.h"

FQFNetworkMonitorThread::FQFNetworkMonitorThread(FQFMedia *m)
{
	media = m;
}

FQFNetworkMonitorThread::~FQFNetworkMonitorThread()
{
}

bool FQFNetworkMonitorThread::getConnect()
{
	return isConnect;
}

void FQFNetworkMonitorThread::setRtspUrl(const char * url)
{
	strcpy(rtspUrl, url);
}

void FQFNetworkMonitorThread::startMonitor()
{
	isMonitor = true;
	if (!this->isRunning())
		this->start();
}

void FQFNetworkMonitorThread::stopMonitor()
{
	isMonitor = false;
}

void FQFNetworkMonitorThread::realseMonitor()
{
	isExit = true;
}

void FQFNetworkMonitorThread::run()
{
	while (!isExit)
	{
		if (!media)
		{
			msleep(10);
			continue;
		}
		if (!isMonitor)
		{
			msleep(10);
			continue;
		}
		bool ok = media->getTimeoutState();
		if (!ok)
		{
			isConnect = media->openStream(rtspUrl);
			
			if (!isConnect)
			{
				msleep(10000);
				continue;
			}
			else
			{
				msleep(50);
			}
		}
		isConnect = ok;
		msleep(1000);
	}
}
