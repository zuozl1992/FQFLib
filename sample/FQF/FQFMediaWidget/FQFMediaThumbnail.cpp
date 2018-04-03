#include "FQFMediaThumbnail.h"
#include <QPainter>
FQFMediaThumbnail::FQFMediaThumbnail(QWidget *parent):
	QWidget(parent)
{
	media = new FQFMedia;
	showTimer = new QTimer;
	connect(showTimer, SIGNAL(timeout()),
		this, SLOT(showTimerTimeout()));
}

FQFMediaThumbnail::~FQFMediaThumbnail()
{
}

bool FQFMediaThumbnail::openStream(const char * url)
{
	if (showTimer->isActive())
		return false;
	bool ok = media->openStream(url);
	if (!ok)
		return false;
	showTimer->start(10);
    imageNum = 0;
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

void FQFMediaThumbnail::showTimerTimeout()
{
	this->update();
}

void FQFMediaThumbnail::paintEvent(QPaintEvent *)
{
	if (imageNum < 5)
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
	else if (imageNum == 5)
	{
		imageNum++;
		media->closeStream();
	}
	else
	{
		showTimer->stop();
		int dw = width();
		int dh = height();
		int tw, th;
		if (!media->getYuvSize(&tw, &th))
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

