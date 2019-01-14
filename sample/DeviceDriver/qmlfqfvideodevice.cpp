#include "qmlfqfvideodevice.h"

QmlFQFVideoDevice::QmlFQFVideoDevice(QObject *parent) : QObject(parent)
  , QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

bool QmlFQFVideoDevice::openDevice(int width, int height)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
    return true;
}

void QmlFQFVideoDevice::closeDevice()
{

}

void QmlFQFVideoDevice::exitDevice()
{

}

void QmlFQFVideoDevice::writeToDeviceBuffer(unsigned char *data, int width, int height)
{
    QImage image(data,width,height,QImage::Format_ARGB32);
    pig = QPixmap::fromImage(image);
    emit callQmlRefeshImg();
}

QPixmap QmlFQFVideoDevice::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);
    Q_UNUSED(size);
    Q_UNUSED(requestedSize);
    return pig;
}
