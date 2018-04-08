#pragma once

#include <QThread>
#include "FQF/FQFMedia/FQFMedia.h"
class FQFMediaPlayer :
	public QThread
{
public:
	FQFMediaPlayer();
	virtual ~FQFMediaPlayer();
	bool openStream(const char *url);
	void closeStream();
	bool yuvToRgb(char *outSpace, int outWidth, int outHeight);
	std::string getError();
	bool getYuvSize(int *w, int *h);
	void setStreamState(bool state);
    void setRtspTransport(bool state);
	bool getTimeoutState();
	void setOpenInputTimeout(int time);
	void setReadFrameTimeout(int time);
	void run();
protected:
	FQFMedia *media = NULL;
	bool isExit = true;
	bool isPlay = true;
};

