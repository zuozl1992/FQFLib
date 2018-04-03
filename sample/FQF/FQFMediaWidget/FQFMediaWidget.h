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
	bool openStream(const char *url, int fps = 25);
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
	void streamConnectStateChanged(bool state);
	void streamPlayStateChanged(bool state);

protected:
	void paintEvent(QPaintEvent *e);
	void mouseReleaseEvent(QMouseEvent *);

	FQFMediaPlayer *player = NULL;
	QImage *image = NULL;
	QImage *saveImage = NULL;
	QTimer *updateTimer = NULL;
	QFutureWatcher<bool> watcher;
	int w = 0;
	int h = 0;
	int dw = 0;
	int dh = 0;
	int fps = 8;
	bool connectState = false;
	bool playState = false;
};

