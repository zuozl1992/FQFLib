#ifndef QMLFQFVIDEODEVICE_H
#define QMLFQFVIDEODEVICE_H

#include <QObject>
#include <QImage>
#include <QQuickImageProvider>
#include "fqfvideodevice.h"

class QmlFQFVideoDevice : public QObject , public FQF::FQFVideoDevice , public QQuickImageProvider
{
    Q_OBJECT
public:
    QmlFQFVideoDevice(QObject *parent = nullptr);
    //打开视频设备，该操作会先执行关闭操作。
    //@param width 视频宽度
    //@param height 视频高度
    virtual bool openDevice(int width , int height);
    //关闭设备，该操作会关闭设备，释放内存
    virtual void closeDevice();
    virtual void exitDevice();
    //向播放设备写入视频数据，写入数据要求是解码完成的rgb数据,要求数据连续提供
    //@param data rgba数据，data空间由该函数释放
    //@param width 图像的宽
    //@param height 图像的高
    virtual void writeToDeviceBuffer(unsigned char *data,int width,int height);
    //向qml传递文件
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

signals:
    void callQmlRefeshImg();

protected:
    QPixmap pig;
};

#endif // QMLFQFVIDEODEVICE_H
