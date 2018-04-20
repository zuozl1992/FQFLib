#ifndef _FQFMEDIA_H_
#define _FQFMEDIA_H_

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
	void freePacket(AVPacket *pkt);
	bool decodePacket(const AVPacket *pkt);
	bool yuvToRgb(char *outSpace, int outWidth, int outHeight);	
	std::string getError();
	bool packetIsVideo(int streamIndex);
	bool getYuvSize(int *w,int *h);
    void setRtspTransport(bool state);
	bool getTimeoutState();
	void setOpenInputTimeout(int time);
	void setReadFrameTimeout(int time);

	time_t getStartTime();
	void setTimeoutState(bool state);
	int getOpenInputTimeout();
	int getReadFrameTimeout();
	bool isReadFrame();

protected:
	QMutex *locker = NULL;
	AVFormatContext *inFmtCtx = NULL;
	AVFrame *videoFrame = NULL;
	SwsContext *videoSwsCtx = NULL;
	time_t startTime;
	char errorbuf[1024];
	char rtspUrl[1024];
	bool bRtspTransport = true;
	bool bTimeoutState = false;
	bool bIsReadFrame = false;
	int iVideoStream = -1;
	int iWidth = 0;
	int iHeight = 0;
	int iOpenInputTimeout = 5;
	int iReadFrameTimeout = 3;
	int readErrorNum = 0;
	int decodeErrorNum = 0;
	int swsErrorNum = 0;
};

#endif