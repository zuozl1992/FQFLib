#pragma once

#include <QWidget>
#include <QTimer>
#include "FQF/FQFMedia/FQFMedia.h"
class FQFMediaThumbnail :
	public QWidget
{
	Q_OBJECT
public:
	FQFMediaThumbnail(QWidget *parent = Q_NULLPTR);
	virtual ~FQFMediaThumbnail();
	bool openStream(const char *url);
    double getStreamProportion();
	void setRtspTransport(bool state);
	void setOpenInputTimeout(int time);

protected slots:
	void showTimerTimeout();

signals:
	void clicked();

protected:
	void paintEvent(QPaintEvent *);
	void mouseReleaseEvent(QMouseEvent *);
    FQFMedia *media = NULL;
	QImage *image = NULL;
	QTimer *showTimer = NULL;
	int imageNum = 0;
};

