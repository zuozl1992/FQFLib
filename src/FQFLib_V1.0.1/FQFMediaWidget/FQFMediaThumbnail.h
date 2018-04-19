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
	void openStream(const char *url);
    double getStreamProportion();
	void setRtspTransport(bool state);
	void setOpenInputTimeout(int time);

protected slots:
	void showTimerTimeout();
	void openStreamSlot();

signals:
	void clicked();

protected:
	void paintEvent(QPaintEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void readStream();
	
    FQFMedia *media = NULL;
	QImage *image = NULL;
	QTimer *showTimer = NULL;
	int imageNum = 0;
	QFutureWatcher<bool> openWatcher;
};

