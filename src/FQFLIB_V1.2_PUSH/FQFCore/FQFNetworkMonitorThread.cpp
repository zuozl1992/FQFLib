#include "FQFNetworkMonitorThread.h"
#include <QDateTime>
#include <myhttp.h>
#include "journal.h"
#include "configure.h"
#include "relist.h"
#include "infoOutput.h"
#define MEDIA (*media)

FQFNetworkMonitorThread::FQFNetworkMonitorThread(FQFMedia **m)
{
    media = m;
    isExit = true;
    srand(Configure::getMySrand());
    connect(ReList::getObject(),SIGNAL(reConnect(int)),
            this,SLOT(reConnectSlot(int)));
}

FQFNetworkMonitorThread::~FQFNetworkMonitorThread()
{
}

void FQFNetworkMonitorThread::setRtspUrl(const char * url)
{
	strcpy(rtspUrl, url);
}

void FQFNetworkMonitorThread::setRtmpUrl(const char * url)
{
	strcpy(rtmpUrl, url);
    generatingRandomAddress();
}

void FQFNetworkMonitorThread::setRtspId(int id)
{
    cameraId = id;
}

void FQFNetworkMonitorThread::startMonitor()
{
    isExit = false;
	isMonitor = true;
	if (!this->isRunning())
		this->start();
}

void FQFNetworkMonitorThread::stopMonitor()
{
	isMonitor = false;
}

int FQFNetworkMonitorThread::getCameraId()
{
    return cameraId;
}

QString FQFNetworkMonitorThread::getRushUrl()
{
    QString re = rtmpUrl;
    return re;
}

void FQFNetworkMonitorThread::reConnectSlot(int id)
{
    if(id == cameraId)
        isRec = true;
}

void FQFNetworkMonitorThread::run()
{
	while (!isExit)
	{
		if (!isMonitor)
		{
			msleep(10);
			continue;
        }
        if (!MEDIA)
        {
            msleep(10);
            continue;
        }
        //InfoOutput::putOutInfo(tr("%1,%2").arg(cameraId).arg(MEDIA->getTimeoutState()));

        if (!(MEDIA->getTimeoutState()))
        {
            MEDIA->closeStream();
            if(!isRec)
            {
                if(isFirstRe)
                {
                    ReList::getObject()->addReConnectToList(cameraId);
                    isFirstRe = false;
                }
                msleep(10);
                continue;
            }
            else
            {
                isRec = false;
                isFirstRe = true;
                bool bRe;
                if (!MEDIA)
                {
                    msleep(10);
                    continue;
                }
                bRe = MEDIA->openInStream(rtspUrl);
                if (!bRe)
                {
                    msleep(10000);
                    continue;
                }
                bRe = MEDIA->openOutStream(rtmpUrl);

                if (!bRe)
                {
                    updateRandomAddress();
                    msleep(10000);
                    continue;
                }
            }
        }

        else
        {
            isRec = false;
        }
//        InfoOutput::putOutInfo("123");
//        bool te = MEDIA->openTestStream(rtmpUrl);
//        InfoOutput::putOutInfo(tr("id:%1,state:%2")
//                               .arg(cameraId)
//                               .arg(te));
        msleep(1000);
	}
}

void FQFNetworkMonitorThread::generatingRandomAddress()
{
	int l = strlen(rtmpUrl);
    if(rtmpUrl[l-1] != '/')
    {
        rtmpUrl[l] = '/';
        rtmpUrl[l+1] = '\0';
        l++;
    }
//    strcat(rtmpUrl,QString(tr("Id=%1&Key=").arg(cameraId)).toStdString().c_str());
//    l = strlen(rtmpUrl);
    char temp;
    quint16 i;
    for (i = l; i < l + 8; i++)
    {
        temp = rand() % 52;
        rtmpUrl[i] = temp > 25 ? (temp -26 + 'a') : (temp + 'A');
    }
    rtmpUrl[i] = '\0';
}
void FQFNetworkMonitorThread::updateRandomAddress()
{
    srand(Configure::getMySrand());
	int l = strlen(rtmpUrl);
    int temp;
    quint16 i;
    for (i = l-8; i < l; i++)
	{
		temp = rand() % 52;
		rtmpUrl[i] = temp > 25 ? (temp - 26 + 'a') : (temp + 'A');
	}
    rtmpUrl[i] = '\0';
}
