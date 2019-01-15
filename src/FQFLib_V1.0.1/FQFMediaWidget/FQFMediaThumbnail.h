#pragma once

#include <QOpenGLWidget>
#include <QWidget>
#include <QTimer>
#include <QFutureWatcher>
#include "FQF/FQFMedia/FQFMedia.h"
class FQFMediaThumbnail :
	public QOpenGLWidget
{
	Q_OBJECT
public:
	FQFMediaThumbnail(QWidget *parent = Q_NULLPTR);
	virtual ~FQFMediaThumbnail();
    //打开媒体获取图片，如需刷新可以多次调用
	//@parma url rtsp地址
	//@return bool 操作结果
	void openStream(const char *url);
    //获取视频比例
    //@return double 宽/高
    double getStreamProportion();
	//设置RTSP打开方式，默认tcp，可通过此函数更改打开方式，在openStream之前调用才会生效
	//@param state true tcp,false udp
	void setRtspTransport(bool state);
	//设置打开流媒体的超时时间，默认10S，需在打开之前设置
	//@param time 超时时间 单位S
	void setOpenInputTimeout(int time);

protected slots:
	//刷新计时器
	void showTimerTimeout();

	void openStreamSlot();	//打开

signals:
	//点击信号
	void clicked();

protected:
	//绘图事件
	void paintEvent(QPaintEvent *);
	//鼠标抬起事件
	void mouseReleaseEvent(QMouseEvent *);

	void readStream();
    FQFMedia *media = NULL;     //媒体
	QImage *image = NULL;		//图片
	QTimer *showTimer = NULL;	//刷新定时器
	int imageNum = 0;			//取图次数
	QFutureWatcher<bool> openWatcher;		//打开异步线程
};

