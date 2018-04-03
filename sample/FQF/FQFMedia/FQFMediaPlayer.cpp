#include "FQFMediaPlayer.h"
#include <iostream>
using namespace std;

FQFMediaPlayer::FQFMediaPlayer()
{
	media = new FQFMedia;
}

FQFMediaPlayer::~FQFMediaPlayer()
{
}

bool FQFMediaPlayer::openStream(const char * url)
{
	bool ok = media->openStream(url);
	if (!ok)
		return false;
	isExit = false;
	this->start();
	return true;
}

void FQFMediaPlayer::closeStream()
{
	isExit = true;
	media->closeStream();
}

AVPacket FQFMediaPlayer::readStream()
{
	return media->readStream();
}

bool FQFMediaPlayer::decodePacket(const AVPacket * pkt)
{
	return media->decodePacket(pkt);
}

bool FQFMediaPlayer::yuvToRgb(char * outSpace, int outWidth, int outHeight)
{
	return media->yuvToRgb(outSpace, outWidth, outHeight);
}

std::string FQFMediaPlayer::getError()
{
	return media->getError();
}

bool FQFMediaPlayer::packetIsVideo(int streamIndex)
{
	return media->packetIsVideo(streamIndex);
}

bool FQFMediaPlayer::getYuvSize(int * w, int * h)
{
	return media->getYuvSize(w, h);
}

void FQFMediaPlayer::setStreamState(bool state)
{
    isPlay = state;
}

void FQFMediaPlayer::setRtspTransport(bool state)
{
    media->setRtspTransport(state);
}

bool FQFMediaPlayer::getTimeoutState()
{
	return media->getTimeoutState();
}

void FQFMediaPlayer::setOpenInputTimeout(int time)
{
	media->setOpenInputTimeout(time);
}

void FQFMediaPlayer::setReadFrameTimeout(int time)
{
	media->setReadFrameTimeout(time);
}

void FQFMediaPlayer::run()
{
	while (!isExit && media->getTimeoutState())
	{
		if (!isPlay)
		{
			msleep(10);
			continue;
		}
		AVPacket pkt = media->readStream();
		if (pkt.size <= 0)
		{
			msleep(10);
			continue;
		}
		if (media->packetIsVideo(pkt.stream_index))
		{
			media->decodePacket(&pkt);
			av_packet_unref(&pkt);
			continue;
		}
		av_packet_unref(&pkt);
		
	}
}
