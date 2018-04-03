#include <QPainter>
#include <QtConcurrent/QtConcurrent>
#include <QMouseEvent>
#include "FQFMediaWidget.h"

FQFMediaWidget::FQFMediaWidget(QWidget *parent)
	:QOpenGLWidget(parent)
{
	player = new FQFMediaPlayer;
    updateTimer = new QTimer;
	connect(updateTimer, SIGNAL(timeout()),
		this, SLOT(updateTimerTimeout()));
	connect(&swscalWatcher, SIGNAL(finished()),
		this, SLOT(updatePage()));
	connect(&openWatcher, SIGNAL(finished()),
		this, SLOT(openStreamSlot()));
}


FQFMediaWidget::~FQFMediaWidget()
{
}

void FQFMediaWidget::openStream(const char * url, int f)
{
	this->fps = f;
	if(!openWatcher.isRunning())	//异步打开
		openWatcher.setFuture(QtConcurrent::run(player, &FQFMediaPlayer::openStream, url));
	isReconnect = true;
	strcpy(rtspUrl, url);
}

void FQFMediaWidget::openStream(QString admin, QString passwd, QString addr, qint16 port, quint8 f)
{
	this->fps = f;
	QString url = tr("rtsp://%1:%2@%3:%4").arg(admin, passwd, addr).arg(port);
	if (!openWatcher.isRunning())	//异步打开
		openWatcher.setFuture(QtConcurrent::run(player, &FQFMediaPlayer::openStream, url.toStdString().c_str()));
	isReconnect = true;
	strcpy(rtspUrl, url.toStdString().c_str());
}

void FQFMediaWidget::closeStream()
{
	updateTimer->stop();
	player->closeStream();
	if (playState != false)
	{
		emit streamPlayStateChanged(false);
		playState = false;
	}
}

void FQFMediaWidget::setStreamState(bool t)
{
	if (t)
	{
		updateTimer->start(1000 / fps);
	}
	else
	{
		updateTimer->stop();;
	}
	if (playState != t)
	{
		emit streamPlayStateChanged(t);
		playState = t;
	}
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
	if (!player->getYuvSize(&tw, &th))
		return false;
	if (saveImage == NULL)
	{
		uchar *buf = new uchar[tw * th * 4];
		saveImage = new QImage(buf, tw, th, QImage::Format_ARGB32);
	}
	bool ok = player->yuvToRgb((char *)saveImage->bits(), tw, th);
	if (!ok)
		return false;
	return saveImage->save(path);
}

void FQFMediaWidget::updatePage()
{
	if (swscalWatcher.future().result() == true)
	{
		this->update();
	}
}

void FQFMediaWidget::openStreamSlot()
{
	bool ok = openWatcher.future().result();
	if (playState != ok)
	{
		emit streamPlayStateChanged(ok);
		playState = ok;
	}
	if (ok)
	{
		updateTimer->start(1000 / fps);
	}
	else
	{
		reconnect();
	}
		
}

void FQFMediaWidget::paintEvent(QPaintEvent * event)
{
	if (!image)
		return;
	int dw = width();
	int dh = height();
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
	QPainter painter;
	painter.begin(this);
	painter.drawImage(QRect((width() - dw) / 2, (height() - dh) / 2, dw, dh), *image);
	painter.end();
}

void FQFMediaWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		emit clicked();
	}
}

void FQFMediaWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		emit doubleClicked();
	}
}

void FQFMediaWidget::reconnect()
{
	if (isReconnect)
	{
		if (!openWatcher.isRunning())	//异步打开
			openWatcher.setFuture(QtConcurrent::run(player, &FQFMediaPlayer::openStream, (const char *)rtspUrl));
		isReconnect = false;
	}
}

void FQFMediaWidget::checkConnectState()
{
	if (player->getTimeoutState() != connectState)
	{
		emit streamConnectStateChanged(!connectState);
		connectState = !connectState;
		if (!connectState)
		{
			if (updateTimer->isActive())
				updateTimer->stop();
			if (swscalWatcher.isRunning())
				swscalWatcher.finished();
			initImage();
			this->update();
		}
	}
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
	if (!player->getYuvSize(&tw, &th))
		return false;
	w = width();
	h = height();
	sw = width();
	sh = height();
	iw = tw;
	ih = th;
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

void FQFMediaWidget::updateTimerTimeout()
{
	checkConnectState();
	checkWidgetSize();
	if (!image)
	{
		if (!initImage())
		{
			return;
		}
	}
	if (!swscalWatcher.isRunning())//放到异步线程中处理。
	{
		swscalWatcher.setFuture(QtConcurrent::run(player, &FQFMediaPlayer::yuvToRgb, (char *)image->bits(), sw, sh));
	}
}

double FQFMediaWidget::getStreamProportion()
{
    int tw,th;
    if(player->getYuvSize(&tw,&th))
    {
        return (double)tw/(double)th;
    }
    return 0;
}

void FQFMediaWidget::setRtspTransport(bool state)
{
	player->setRtspTransport(state);
}

bool FQFMediaWidget::getTimeoutState()
{
	return player->getTimeoutState();
}

void FQFMediaWidget::setOpenInputTimeout(int time)
{
	player->setOpenInputTimeout(time);
}

void FQFMediaWidget::setReadFrameTimeout(int time)
{
	player->setReadFrameTimeout(time);
}
