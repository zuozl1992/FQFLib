#include <QPainter>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>
#include "FQFMediaWidget.h"

FQFMediaWidget::FQFMediaWidget(QWidget *parent)
	:QOpenGLWidget(parent)
{
	player = new FQFMediaPlayer;
    updateTimer = new QTimer;
	connect(updateTimer, SIGNAL(timeout()),
		this, SLOT(updateTimerTimeout()));
	connect(&watcher, SIGNAL(finished()),
		this, SLOT(updatePage()));
}


FQFMediaWidget::~FQFMediaWidget()
{
}

bool FQFMediaWidget::openStream(const char * url, int f)
{
	bool ok = player->openStream(url);
	if (!ok)
		return false;
	this->fps = f;
	updateTimer->start(1000 / fps);
	if (playState != true)
	{
		emit streamPlayStateChanged(true);
		playState = true;
	}
	return true;
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
	if (watcher.future().result() == true && 
		width() >= dw && height() >= dh)
	{
		QPainter painter;
		painter.begin(this);
		painter.drawImage(QRect((width() - dw) / 2, (height() - dh) / 2, dw, dh), *image);
		painter.end();
	}
}

void FQFMediaWidget::paintEvent(QPaintEvent * e)
{
	if (player->getTimeoutState() != connectState)
	{
		emit streamConnectStateChanged(!connectState);
		connectState = !connectState;
		if (!connectState)
		{
			if (updateTimer->isActive())
				updateTimer->stop();
			if (watcher.isRunning())
				watcher.finished();
		}
	}
	if (w != width() || h != height())
	{
		if (watcher.isRunning())
			watcher.finished();
		if (image)
		{
			delete image->bits();
			delete image;
			image = NULL;
		}
	}
	if (image == NULL)
	{
		dw = width();
		dh = height();
		w = width();
		h = height();
		int tw, th;
		if (!player->getYuvSize(&tw, &th))
			return;
		if ((double)width() / (double)height() != (double)tw / (double)th)
		{
			double pro = (double)tw / (double)th;
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
		uchar *buf = new uchar[dw * dh * 4];
		image = new QImage(buf, dw, dh, QImage::Format_ARGB32);
	}
	if (!watcher.isRunning())
	{
		watcher.setFuture(QtConcurrent::run(player, &FQFMediaPlayer::yuvToRgb, (char *)image->bits(), dw, dh));
    }
}

void FQFMediaWidget::mouseReleaseEvent(QMouseEvent *)
{
	emit clicked();
}

void FQFMediaWidget::updateTimerTimeout()
{
	this->update();
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
