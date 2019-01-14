#pragma once
struct AVFrame;
namespace FQF {
class FQFVideoDevice
{
public:
    FQFVideoDevice(){}
    virtual ~FQFVideoDevice(){}
    //打开视频设备，该操作会先执行关闭操作。
    //@param width 视频宽度
    //@param height 视频高度
    virtual bool openDevice(int width , int height) = 0;
    //关闭设备，该操作会关闭设备
    virtual void closeDevice() = 0;
    //退出设备，该操作会退出设备，释放内存
    virtual void exitDevice() = 0;
    //向播放设备写入视频数据，写入数据要求是解码完成的rgb数据,格式AV_PIX_FMT_BGRA
    //@param data rgba数据，data空间由该函数释放
    //@param width 图像的宽
    //@param height 图像的高
    virtual void writeToDeviceBuffer(unsigned char *data,int width,int height) = 0;
};
}
