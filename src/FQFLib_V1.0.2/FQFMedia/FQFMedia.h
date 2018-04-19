#pragma once

#include <string>
#include <QMutex>
extern "C"
{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
class FQFMedia
{
public:
	FQFMedia();
	virtual ~FQFMedia();
	bool openStream(const char *url);
	void closeStream();
	AVPacket readStream();
	bool decodePacket(const AVPacket *pkt);
	bool yuvToRgb(char *outSpace, int outWidth, int outHeight);	
	std::string getError();
	bool packetIsVideo(int streamIndex);
	bool getYuvSize(int *w,int *h);
    time_t getStartTime();
    void setRtspTransport(bool state);
	bool getTimeoutState();
	void setTimeoutState(bool state);
	void setOpenInputTimeout(int time);
	int getOpenInputTimeout();
	void setReadFrameTimeout(int time);
	int getReadFrameTimeout();
	bool isReadFrame();
	bool reconnect();

protected:
	AVFormatContext *iCtx = NULL;
	AVFrame *yuv = NULL;
	SwsContext *vCtx = NULL;
	QMutex mutex;
	char errorbuf[1024];
	char rtspUrl[1024];
	bool rtspTransport = true;
	bool timeoutState = false;
	bool bIsReadFrame = false;
	time_t startTime;
	int reconnectionNum = 0;
	int videoStream = -1;
	int yuvWidth = 0;
	int yuvHeight = 0;
	int openInputTimeout = 10;
	int readFrameTimeout = 3;
	
};

