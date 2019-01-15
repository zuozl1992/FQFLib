#pragma once

#include <QOpenGLWidget>
#include <QWidget>
#include <QTimer>
#include <QFutureWatcher>
#include "FQF/FQFMedia/FQFMediaPlayer.h"
class FQFMediaWidget :
	public QOpenGLWidget
{
	Q_OBJECT
public:
	//构造函数，初始化相关变量，绑定信号
	FQFMediaWidget(QWidget *parent = Q_NULLPTR);
	virtual ~FQFMediaWidget();
	//打开视频
	//@param url rtsp地址
	//@param fps 网络视频的fps，直播视频无法获取，需手动设置，默认为25
	//@return bool 操作结果
	void openStream(const char *url, int fps = 25);
	//关闭视频
	void closeStream();
	//设置视频状态，暂停/播放
	//@param t状态 true 播放 false 暂停
	void setStreamState(bool t);
	//保存截图
	//@param path 截图保存路径
	bool saveScreenshot(const QString &path);
    //获取视频比例
    //@return double 宽/高
    double getStreamProportion();
	//设置RTSP打开方式，默认tcp，可通过此函数更改打开方式，在openStream之前调用才会生效
	//@param state true tcp,false udp
	void setRtspTransport(bool state);
	//获取当前连接状态
	//@return 连接正常true，异常false
	bool getTimeoutState();
	//设置打开流媒体的超时时间，默认10S，需在打开之前设置
	//@param time 超时时间 单位S
	void setOpenInputTimeout(int time);
	//设置读取packet的超时时间，默认3S，需在打开之前设置
	//@param time 超时时间 单位S
	void setReadFrameTimeout(int time);

protected slots:
	//界面刷新定时器槽函数
	void updateTimerTimeout();
	//异步线程完成触发信号槽函数
	void updatePage();	//刷新

	void openStreamSlot();	//打开

signals:
	//点击信号
	void clicked();
	//视频连接状态改变
	void streamConnectStateChanged(bool state);
	//视频播放状态改变
	void streamPlayStateChanged(bool state);

protected:
	//绘图事件，获取视频解码图像并绘制到当前界面
	void paintEvent(QPaintEvent *e);
	//鼠标抬起事件
	void mouseReleaseEvent(QMouseEvent *);

	FQFMediaPlayer *player = NULL;			//媒体播放器对象
	QImage *image = NULL;					//播放转码对象
	QImage *saveImage = NULL;				//截图对象
	QTimer *updateTimer = NULL;				//刷新界面定时器
	QFutureWatcher<bool> watcher;			//转码异步线程
	QFutureWatcher<bool> openWatcher;		//打开异步线程
	int w = 0;								//当界面的宽
	int h = 0;								//当界面的高
	int dw = 0;								//当前绘制图像的宽
	int dh = 0;								//当前绘制图像的高
	int fps = 8;							//默认的解码频率
	bool connectState = false;
	bool playState = false;
};

