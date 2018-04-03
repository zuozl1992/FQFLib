#include "FQFMediaThumbnail.h"
#include <QPainter>
#include <QtConcurrent/QtConcurrent>
FQFMediaThumbnail::FQFMediaThumbnail(QWidget *parent):
	QOpenGLWidget(parent)
{
	media = new FQFMedia;
	showTimer = new QTimer;
	connect(showTimer, SIGNAL(timeout()),
		this, SLOT(showTimerTimeout()));
	connect(&openWatcher, SIGNAL(finished()),
		this, SLOT(openStreamSlot()));
}

FQFMediaThumbnail::~FQFMediaThumbnail()
{
}

void FQFMediaThumbnail::openStream(const char * url)
{
	//如果上一次任务未结束，直接返回
	if (showTimer->isActive())
		return;
	if (!openWatcher.isRunning())
		openWatcher.setFuture(QtConcurrent::run(media, &FQFMedia::openStream, url));
}

double FQFMediaThumbnail::getStreamProportion()
{
    int tw,th;
    if(media->getYuvSize(&tw,&th))
    {
        return (double)tw/(double)th;
    }
    return 0;
}

void FQFMediaThumbnail::setRtspTransport(bool state)
{
	media->setRtspTransport(state);
}

void FQFMediaThumbnail::setOpenInputTimeout(int time)
{
	media->setOpenInputTimeout(time);
}

void FQFMediaThumbnail::openStreamSlot()
{
	bool ok = openWatcher.future().result();
	if (!ok)
		return;
	//启动定时器
	showTimer->start(10);
	//截图计数器清零
	imageNum = 0;
}

void FQFMediaThumbnail::showTimerTimeout()
{
	if (imageNum < 5)
	{
		readStream();
	}
	else if (imageNum == 5)
	{
		imageNum++;
		media->closeStream();
	}
	else
	{
		showTimer->stop();
		this->update();
	}
}

void FQFMediaThumbnail::paintEvent(QPaintEvent *)
{
	if (imageNum > 5)
	{
		int dw = width();
		int dh = height();
		int tw, th;
		if (!media->getYuvSize(&tw, &th))
			return;
		//校验图片比例
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
		QPainter painter;
		painter.begin(this);
		painter.drawImage(QRect((width() - dw) / 2, (height() - dh) / 2, dw, dh), *image);
		painter.end();
	}
}

void FQFMediaThumbnail::mouseReleaseEvent(QMouseEvent *)
{
	emit clicked();
}

void FQFMediaThumbnail::readStream()
{
	AVPacket pkt = media->readStream();
	if (pkt.size <= 0)
	{
		return;
	}
	if (media->packetIsVideo(pkt.stream_index))
	{
		media->decodePacket(&pkt);
		av_packet_unref(&pkt);
	}
	else
	{
		av_packet_unref(&pkt);
		return;
	}
	int tw, th;
	if (!media->getYuvSize(&tw, &th))
		return;
	if (image)
	{
		delete image->bits();
		delete image;
		image = NULL;
	}
	uchar *buf = new uchar[tw * th * 4];
	image = new QImage(buf, tw, th, QImage::Format_ARGB32);
	media->yuvToRgb((char *)image->bits(), tw, th);
	imageNum++;
}

