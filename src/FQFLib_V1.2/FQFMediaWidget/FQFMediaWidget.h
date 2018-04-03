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
	FQFMediaWidget(QWidget *parent = Q_NULLPTR);
	virtual ~FQFMediaWidget();
	void openStream(const char *url, int fps = 25);
	void openStream(QString admin, QString passwd, QString addr, qint16 port = 554, quint8 fps = 25);
	void closeStream();
	void setStreamState(bool t);
	bool saveScreenshot(const QString &path);
    double getStreamProportion();
	void setRtspTransport(bool state);
	bool getTimeoutState();
	void setOpenInputTimeout(int time);
	void setReadFrameTimeout(int time);

protected slots:
	void updateTimerTimeout();
	void updatePage();
	void openStreamSlot();

signals:
	void clicked();
	void doubleClicked();
	void streamConnectStateChanged(bool state);
	void streamPlayStateChanged(bool state);

protected:
	void paintEvent(QPaintEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void reconnect();
	void checkConnectState();
	void checkWidgetSize();
	bool initImage();

	FQFMediaPlayer *player = NULL;
	QImage *image = NULL;
	QImage *saveImage = NULL;
	QTimer *updateTimer = NULL;
	char rtspUrl[1024] = { 0 };
	QFutureWatcher<bool> swscalWatcher;
	QFutureWatcher<bool> openWatcher;
	int w = 0;
	int h = 0;
	int iw = 0;
	int ih = 0;
	int sw = 0;
	int sh = 0;
	int fps = 8;
	bool connectState = false;
	bool playState = false;
	bool isReconnect = true;
};

