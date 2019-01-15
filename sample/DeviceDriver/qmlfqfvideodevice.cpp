#include "qmlfqfvideodevice.h"
#include <QDebug>

QmlFQFVideoDevice::QmlFQFVideoDevice(QObject *parent) : QObject(parent)
  , QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

bool QmlFQFVideoDevice::openDevice(int width, int height)
{
    closeDevice();
    mux.lock();
    this->width = width;
    this->height = height;
    mux.unlock();
    return true;
}

void QmlFQFVideoDevice::closeDevice()
{
    mux.lock();
    width = 500;
    height = 500;
    mux.unlock();
}

void QmlFQFVideoDevice::exitDevice()
{
    mux.lock();
    width = 500;
    height = 500;
    mux.unlock();
}

void QmlFQFVideoDevice::writeToDeviceBuffer(unsigned char *data, int width, int height)
{
    mux.lock();
    QImage image(data,width,height,QImage::Format_ARGB32);
    pig = QPixmap::fromImage(image);
    mux.unlock();
    emit callQmlRefeshImg();
}

void QmlFQFVideoDevice::setPause(bool isPause)
{
    Q_UNUSED(isPause);
}

void QmlFQFVideoDevice::writeToDeviceBuffer(QImage img)
{
    mux.lock();
    pig = QPixmap::fromImage(img);
    mux.unlock();
    emit callQmlRefeshImg();
}

QPixmap QmlFQFVideoDevice::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);
    Q_UNUSED(size);
    Q_UNUSED(requestedSize);
    return pig;
}

int QmlFQFVideoDevice::getVideoWidth()
{
    return width;
}

int QmlFQFVideoDevice::getVideoHeight()
{
    return height;
}
