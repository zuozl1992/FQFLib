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
	void openStream(QString admin, QString passwd, QString addr, qint16 port = 554, quint8 fps = 25);
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
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	bool readPkt(int i);
	void reconnect();

    FQFMedia *media = NULL;
	QImage *image = NULL;
	char rtspUrl[1024] = {0};
	QFutureWatcher<bool> openWatcher;
	QFutureWatcher<bool> readWatcher;
	bool isReconnect = true;
	bool isShowImage = false;
	int iw = 0;
	int ih = 0;
};

