#pragma once

#include <QThread>
#include "FQFMedia.h"
class FQFMediaPlayer :
	public QThread
{
	Q_OBJECT
public:
	//构造函数，保存播放器指针
	FQFMediaPlayer(FQFMedia *m);
	virtual ~FQFMediaPlayer();
	//设置播放状态
	//@parma state true 解码 false 停止
	void setStreamState(bool state);
	//读取播放状态
	//@return bool true 播放 false 停止
	bool getStreamState(); 
	void realsePlayer();
	void startPlay();
	void stopPlay();
protected:
	//读取、解码线程
	void run();
	FQFMedia *media = NULL;		//播放
	bool isExit = false;		//运行状态
	bool isPlay = true;			//播放状态
};

