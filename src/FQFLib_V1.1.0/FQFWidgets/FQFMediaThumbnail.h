#pragma once

#include <QOpenGLWidget>
#include <QWidget>
#include <QTimer>
#include <QFutureWatcher>
#include "FQFCore/FQFMedia.h"
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
	void openStreamSlot();
	void readPktSlot();

signals:
	void clicked();
	void doubleClicked();
protected:
	void paintEvent(QPaintEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseDoubleClickEvent(QMouseEvent *event);
	bool readPkt(int i);
	void reconnect();

    FQFMedia *media = NULL;
	QImage *image = NULL;
	bool isShowImage = false;
	QFutureWatcher<bool> openWatcher;
	QFutureWatcher<bool> readWatcher;
	int iw = 0;
	int ih = 0;
	char rtspUrl[1024] = {0};
	bool isReconnect = true;
	
};

