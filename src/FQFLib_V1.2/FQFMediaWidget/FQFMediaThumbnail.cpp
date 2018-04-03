#include "FQFMediaThumbnail.h"
#include <QPainter>
#include <QtConcurrent/QtConcurrent>
#include <QMouseEvent>
FQFMediaThumbnail::FQFMediaThumbnail(QWidget *parent):
	QOpenGLWidget(parent)
{
	media = new FQFMedia;
	connect(&openWatcher, SIGNAL(finished()),
		this, SLOT(openStreamSlot()));
	connect(&readWatcher, SIGNAL(finished()),
		this, SLOT(readPktSlot()));
}

FQFMediaThumbnail::~FQFMediaThumbnail()
{
}

void FQFMediaThumbnail::openStream(const char * url)
{
	//如果上一次任务未结束，直接返回
	if (readWatcher.isRunning())
		return;
	if (!openWatcher.isRunning())
		openWatcher.setFuture(QtConcurrent::run(media, &FQFMedia::openStream, url));
	isShowImage = false;
	isReconnect = true;
	strcpy(rtspUrl, url);
}

void FQFMediaThumbnail::openStream(QString admin, QString passwd, QString addr, qint16 port, quint8 fps)
{
	//如果上一次任务未结束，直接返回
	if (readWatcher.isRunning())
		return;
	QString url = tr("rtsp://%1:%2@%3:%4").arg(admin,passwd,addr).arg(port);
	if (!openWatcher.isRunning())
		openWatcher.setFuture(QtConcurrent::run(media, &FQFMedia::openStream, url.toStdString().c_str()));
	isShowImage = false;
	isReconnect = true;
	strcpy(rtspUrl,url.toStdString().c_str());

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
	{
		reconnect();
		return;
	}
	if (!readWatcher.isRunning())
		readWatcher.setFuture(QtConcurrent::run(this, &FQFMediaThumbnail::readPkt, 10));
}

void FQFMediaThumbnail::readPktSlot()
{
	bool ok = readWatcher.future().result();
	if (!ok)
		return;
	isShowImage = true;
	this->update();
}

void FQFMediaThumbnail::paintEvent(QPaintEvent *)
{
	if (isShowImage)
	{
		int dw = width();
		int dh = height();
		//校验界面比例
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
}

void FQFMediaThumbnail::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		emit clicked();
	}
}

void FQFMediaThumbnail::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		emit doubleClicked();
	}
}

bool FQFMediaThumbnail::readPkt(int i)
{
	while (i)
	{
		AVPacket pkt = media->readStream();
		if (pkt.size <= 0)
		{
			continue;
		}
		if (media->packetIsVideo(pkt.stream_index))
		{
			media->decodePacket(&pkt);
			av_packet_unref(&pkt);
		}
		else
		{
			av_packet_unref(&pkt);
			continue;
		}
		int w, h;
		if (!media->getYuvSize(&w, &h))
			return false;
		iw = w;
		ih = h;
		if (image)
		{
			delete image->bits();
			delete image;
			image = NULL;
		}
		uchar *buf = new uchar[iw * ih * 4];
		image = new QImage(buf, iw, ih, QImage::Format_ARGB32);
		if (media->yuvToRgb((char *)image->bits(), iw, ih))
			i--;
	}
}

void FQFMediaThumbnail::reconnect()
{
	//如果上一次任务未结束，直接返回
	if (readWatcher.isRunning())
		return;
	if (isReconnect)
	{
		if (!openWatcher.isRunning())
			openWatcher.setFuture(QtConcurrent::run(media, &FQFMedia::openStream, (const char *)rtspUrl));
		isShowImage = false;
		isReconnect = false;
	}
}
