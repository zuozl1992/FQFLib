#ifndef _FQFMEDIAPUSH_H_
#define _FQFMEDIAPUSH_H_

#include <QThread>
#include "FQFMedia.h"
class FQFMediaPush :
	public QThread
{
	Q_OBJECT
public:
	//构造函数，保存播放器指针
    FQFMediaPush(FQFMedia **m);
	virtual ~FQFMediaPush();
    void startPush();
    void stopPush();
protected:
	//读取、解码线程
	void run();
    FQFMedia **media = NULL;	//播放
	bool isExit = false;		//运行状态
    bool isPush = true;			//播放状态
};

#endif
