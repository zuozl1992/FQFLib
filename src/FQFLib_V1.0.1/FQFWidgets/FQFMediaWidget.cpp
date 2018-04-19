#include <QPainter>
#include <QtConcurrent>
#include <QMouseEvent>
#include "FQFMediaWidget.h"
#include <iostream>
using namespace std;
FQFMediaWidget::FQFMediaWidget(QWidget *parent)
	:QOpenGLWidget(parent)
{
	media = new FQFMedia;
	player = new FQFMediaPlayer(media);
	monitor = new FQFNetworkMonitorThread(media);
    updateTimer = new QTimer;
	connect(updateTimer, SIGNAL(timeout()),
		this, SLOT(updateTimerTimeout()));
	connect(&swscalWatcher, SIGNAL(finished()),
		this, SLOT(updatePage()));
}


FQFMediaWidget::~FQFMediaWidget()
{
	if (swscalWatcher.isRunning())
		swscalWatcher.finished();
	if (updateTimer->isActive())
		updateTimer->stop();
	monitor->realseMonitor();
	player->realsePlayer();
}

void FQFMediaWidget::openStream(const char * url, int f)
{
	monitor->setRtspUrl(url);
	if (monitor->getConnect())
		media->closeStream();
	monitor->startMonitor();
	player->startPlay();
	this->fps = f > 0 ? f : 5;
	updateTimer->start(1000 / fps);
}

void FQFMediaWidget::openStream(QString admin, QString passwd, QString addr, qint16 port, quint8 f)
{
	QString url = tr("rtsp://%1:%2@%3:%4").arg(admin, passwd, addr).arg(port);
	monitor->setRtspUrl(url.toStdString().c_str());
	if (monitor->getConnect())
		media->closeStream();
	monitor->startMonitor();
	player->startPlay();
	this->fps = f > 0 ? f : 5;
	updateTimer->start(1000 / fps);
}

void FQFMediaWidget::closeStream()
{
	updateTimer->stop();
	monitor->stopMonitor();
	if (monitor->getConnect())
		media->closeStream();
	updateTimer->stop();
	player->stopPlay();
	clearWidget();
}

void FQFMediaWidget::setStreamState(bool t)
{
	if (t)
		updateTimer->start(1000 / fps);
	else
		updateTimer->stop();
	player->setStreamState(t);
}

bool FQFMediaWidget::saveScreenshot(const QString & path)
{
	if (saveImage)
	{
		delete saveImage->bits();
		delete saveImage;
		saveImage = NULL;
	}
	int tw, th;
	if (!media->getYuvSize(&tw, &th))
		return false;
	if (saveImage == NULL)
	{
		uchar *buf = new uchar[tw * th * 4];
		saveImage = new QImage(buf, tw, th, QImage::Format_ARGB32);
	}
	bool ok = media->yuvToRgb((char *)saveImage->bits(), tw, th);
	if (!ok)
		return false;
	return saveImage->save(path);
}

void FQFMediaWidget::updatePage()
{
	if (swscalWatcher.future().result() == true)
		this->update();
}

void FQFMediaWidget::paintEvent(QPaintEvent * event)
{
	if (!image)
		return;
	int dw = width();
	int dh = height();
	if (!isShowBlack)
	{
		//校验图片比例
		if ((double)width() / (double)height() != (double)iw / (double)ih)
		{
			double pro = (double)iw / (double)ih;
			if (width() > height() * pro)
			{
				dw = height() * pro;
				dh = height();
			}
			else
			{
				dw = width();
				dh = width() / pro;
			}
		}
	}
	QPainter painter;
	painter.begin(this);
	painter.drawImage(QRect((width() - dw) / 2, (height() - dh) / 2, dw, dh), *image);
	painter.end();
}

void FQFMediaWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
		emit clicked();
}

void FQFMediaWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
		emit doubleClicked();
}

bool FQFMediaWidget::checkConnectState()
{
	bool statue = monitor->getConnect();
	if (statue != isConnectState)
	{
		isConnectState = statue;
		if (!statue)
		{
			clearWidget();
			if (swscalWatcher.isRunning())
				swscalWatcher.finished();
			isShowBlack = true;
			return false;
		}
		else
		{
			if (swscalWatcher.isRunning())
				swscalWatcher.finished();
			if (image)
			{
				delete image->bits();
				delete image;
				image = NULL;
			}
			isShowBlack = false;
			return true;
		}
	}
	if (statue)
	{
		int tw, th;
		if (media->getYuvSize(&tw, &th))
		{
			iw = tw;
			ih = th;
		}
	}
	return statue;
}

void FQFMediaWidget::checkWidgetSize()
{
	if (w != width() || h != height())
	{
		if (swscalWatcher.isRunning())
			swscalWatcher.finished();
		if (image)
		{
			delete image->bits();
			delete image;
			image = NULL;
		}
		
	}
}

bool FQFMediaWidget::initImage()
{
	int tw, th;
	if (!media->getYuvSize(&tw, &th))
		return false;
	w = width();
	h = height();
	sw = width();
	sh = height();
	if ((double)width() / (double)height() != (double)tw / (double)th)
	{
		double pro = (double)tw / (double)th;
		if (width() > height() * pro)
		{
			sw = height() * pro;
			sh = height();
		}
		else
		{
			sw = width();
			sh = width() / pro;
		}
	}
	uchar *buf = new uchar[sw * sh * 4];
	image = new QImage(buf, sw, sh, QImage::Format_ARGB32);
	return true;
}

void FQFMediaWidget::clearWidget()
{
	int iw = width();
	int ih = height();
	if (image)
	{
		delete image->bits();
		delete image;
		image = NULL;
	}
	uchar *buf = new uchar[iw * ih * 4];
	memset(buf, 255, iw * ih * 4);
	for (int i = 0; i < iw * ih * 4; i++)
	{
		if (i % 4 == 3)
		{
			buf[i] = 255;
			continue;
		}
		buf[i] = 0;
	}
	image = new QImage(buf, iw, ih, QImage::Format_ARGB32);
	isShowBlack = true;
	this->update();
}

void FQFMediaWidget::updateTimerTimeout()
{
	if (!checkConnectState())
		return;
	checkWidgetSize();
	if (!image)
		if (!initImage())
			return;
	if (!swscalWatcher.isRunning())//放到异步线程中处理。
		swscalWatcher.setFuture(QtConcurrent::run(media, &FQFMedia::yuvToRgb, (char *)image->bits(), sw, sh));
}

double FQFMediaWidget::getStreamProportion()
{
    int tw,th;
    if(media->getYuvSize(&tw,&th))
        return (double)tw/(double)th;
    return 1;
}

void FQFMediaWidget::setRtspTransport(bool state)
{
	media->setRtspTransport(state);
}

bool FQFMediaWidget::getTimeoutState()
{
	return media->getTimeoutState();
}

void FQFMediaWidget::setOpenInputTimeout(int time)
{
	media->setOpenInputTimeout(time);
}

void FQFMediaWidget::setReadFrameTimeout(int time)
{
	media->setReadFrameTimeout(time);
}
