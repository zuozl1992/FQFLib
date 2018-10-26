#include "FQFMediaPush.h"
#include "infoOutput.h"
#define MEDIA (*media)

FQFMediaPush::FQFMediaPush(FQFMedia **m)
{
	media = m;
    isExit = true;
}

FQFMediaPush::~FQFMediaPush()
{
}

void FQFMediaPush::startPush()
{
	isExit = false;
    isPush = true;
    if(!this->isRunning())
        this->start();
}

void FQFMediaPush::stopPush()
{
    isPush = false;
}

void FQFMediaPush::run()
{
	while (!isExit)
	{
        if (!isPush)
		{
			msleep(10);
			continue;
		}
        if (!MEDIA)
        {
            msleep(10);
            continue;
        }
        if (!MEDIA->getTimeoutState())
		{
			msleep(10);
			continue;
		}
        if (!MEDIA->readStream())
		{
			msleep(10);
			continue;
		}

        if (!MEDIA->pushPacket())
		{
			msleep(10);
            continue;
		}
        msleep(10);
	}
}
