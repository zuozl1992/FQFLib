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
	//打开RTSP媒体获取图片，如需刷新可以多次调用
	//@parma admin rtsp用户名
	//@parma passwd rtsp密码
	//@parma addr rtsp地址
	//@parma port rtsp端口
	//@parma fps rtsp FPS
	//@return bool 操作结果
	void openStream(QString admin, QString passwd, QString addr, qint16 port = 554, quint8 fps = 25);
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
	void openStreamSlot();	//打开异步线程完成槽函数

	void readPktSlot();		//读取异步线程完成槽函数

signals:
	//点击信号
	void clicked();
	//双击信号
	void doubleClicked();

protected:
	//绘图事件
	void paintEvent(QPaintEvent *);
	//鼠标抬起事件
	void mouseReleaseEvent(QMouseEvent *event);
	//鼠标双击事件
	void mouseDoubleClickEvent(QMouseEvent *event);
	//读取帧，为保证流媒体解码效果，需要进行多次读取，直到读取到关键帧建议5~10
	//@param i 读取帧数
	bool readPkt(int i);
	//失败重连，只会重连一次
	void reconnect();

    FQFMedia *media = NULL;					//媒体
	QImage *image = NULL;					//图片
	bool isShowImage = false;				//取图
	QFutureWatcher<bool> openWatcher;		//打开异步线程
	QFutureWatcher<bool> readWatcher;		//读取异步线程
	int iw = 0;								//解码图片的宽
	int ih = 0;								//解码图片的高
	char rtspUrl[1024] = {0};				//打开的rtsp地址
	bool isReconnect = true;				//是否重连
};

