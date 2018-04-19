#pragma once

#include <QThread>
#include "FQFMedia.h"
class FQFMediaPlayer :
	public QThread
{
	Q_OBJECT
public:
	FQFMediaPlayer(FQFMedia *m);
	virtual ~FQFMediaPlayer();
	void setStreamState(bool state);
	bool getStreamState(); 
	void realsePlayer();
	void startPlay();
	void stopPlay();
protected:
	void run();
	FQFMedia *media = NULL;
	bool isExit = false;
	bool isPlay = true;
};

