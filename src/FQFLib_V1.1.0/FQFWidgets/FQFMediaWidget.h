#pragma once

#include <QOpenGLWidget>
#include <QWidget>
#include <QTimer>
#include <QFutureWatcher>
#include "FQFCore/FQFMedia.h"
#include "FQFCore/FQFMediaPlayer.h"
#include "FQFCore/FQFNetworkMonitorThread.h"
class FQFMediaWidget :
	public QOpenGLWidget
{
	Q_OBJECT
public:
	FQFMediaWidget(QWidget *parent = Q_NULLPTR);
	virtual ~FQFMediaWidget();
	void openStream(const char *url, int fps = 8);
	void openStream(QString admin, QString passwd, QString addr, qint16 port = 554, quint8 fps = 8);
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

signals:
	void clicked();
	void doubleClicked();

protected:
	void paintEvent(QPaintEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	bool checkConnectState();
	void checkWidgetSize();
	bool initImage();
	void clearWidget();

	FQFMedia *media = NULL;
	FQFMediaPlayer *player = NULL;
	FQFNetworkMonitorThread *monitor = NULL;
	QImage *image = NULL;
	QImage *saveImage = NULL;
	QTimer *updateTimer = NULL;
	QFutureWatcher<bool> swscalWatcher;
	int w = 0;
	int h = 0;
	int iw = 0;
	int ih = 0;
	int sw = 0;
	int sh = 0;
	int fps = 8;
	bool isConnectState = false;
	bool playState = false;
	bool isShowBlack = false;
};

