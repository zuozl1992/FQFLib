#include "FQFMediaPlayer.h"

FQFMediaPlayer::FQFMediaPlayer(FQFMedia *m)
{
	media = m;
}

FQFMediaPlayer::~FQFMediaPlayer()
{
}

void FQFMediaPlayer::setStreamState(bool state)
{
    isPlay = state;
}

bool FQFMediaPlayer::getStreamState()
{
	return isPlay;
}

void FQFMediaPlayer::realsePlayer()
{
	isExit = true;
}

void FQFMediaPlayer::startPlay()
{
	isExit = false;
	this->start();
}

void FQFMediaPlayer::stopPlay()
{
	isExit = true;
}

void FQFMediaPlayer::run()
{
	while (!isExit)
	{
		msleep(10);
		if (!media)
		{
			msleep(10);
			continue;
		}
		if (!isPlay)
		{
			msleep(10);
			continue;
		}
		if (!media->getTimeoutState())
		{
			msleep(10);
			continue;
		}
		;
		if (!media->readStream())
		{
			msleep(10);
			continue;
		}
		media->decodePacket();
	}
}
