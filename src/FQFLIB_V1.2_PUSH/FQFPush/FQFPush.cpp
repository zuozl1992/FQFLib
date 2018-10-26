#include "FQFPush.h"

FQFPush::FQFPush()
{
	media = new FQFMedia;
    push = new FQFMediaPush(&media);
    monitor = new FQFNetworkMonitorThread(&media);
}

FQFPush::~FQFPush()
{
    monitor->terminate();
    push->terminate();
    delete monitor;
    delete push;
    delete media;
    media = NULL;
}

void FQFPush::startPush(const char *rtspUrl, const char *rtmpUrl, int id)
{
    monitor->setRtspId(id);
    monitor->setRtspUrl(rtspUrl);
	monitor->setRtmpUrl(rtmpUrl);
    media->closeStream();
	monitor->startMonitor();
    push->startPush();
}

void FQFPush::stopPush()
{
    monitor->stopMonitor();
    push->stopPush();

}

bool FQFPush::getState()
{
    return media->getTimeoutState();
}

QString FQFPush::getPushUrl()
{
    return monitor->getRushUrl();
}

int FQFPush::getCameraId()
{
    return monitor->getCameraId();
}

bool FQFPush::getTemp()
{
    if(monitor->isRunning() && push->isRunning())
        return true;
    return false;
}

void FQFPush::reConnect()
{
    media->closeStream();
}
