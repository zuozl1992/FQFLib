#pragma once
#include "fqfdecodethread.h"
#include <mutex>
struct AVCodecParameters;

namespace FQF {
class FQFScale;
class FQFVideoDevice;
class FQFVideoThread : public FQFDecodeThread
{
public:
    FQFVideoThread();
    virtual ~FQFVideoThread();
    virtual void setCallBack(FQFVideoDevice *call);
	//不管成功与否都清理
    virtual bool openDecodeTh(AVCodecParameters *para, int width, int height);
    //关闭解码器，会先调用基类的关闭函数
    //基类的关闭函数会清理缓冲、释放空间、关闭解码器、释放解码器空间、停止线程
    //该函数会清理并释放重采样成员，同时关闭播放设备
    virtual void closeDecodeTh();
    //退出解码器，会先调用基类的退出函数
    //基类的退出函数会清理缓冲、释放空间、退出解码器、释放解码器空间、停止线程
    //该函数会清理并释放重采样成员，同时关闭播放设备
    virtual void exitDecodeTh();
    //清理函数，会先调用基类的清理函数
    //基类的清理函数会清空缓冲、释放缓冲空间
    //-!-该函数会释放播放设备的播放缓冲
    virtual void clearDecodeTh();
    //暂停函数
    //@param isPause 是否暂停
    virtual void setPause(bool isPause);
    //解码pts，如果接收到的pts >= seekpts return true 并且显示 Seek 调用
    //视频跳转需要解析非关键帧到指定位置
    //@param pkt 读到的pkt
    //@param seekpts 参考位置
    virtual bool RepaintPts(AVPacket *pkt, long long seekpts);
    //设置同步时间
    //@param pts 同步时间
    virtual void setSynPts(long long pts);
    //设置播放pts，用于seek后同步音频时间，不会影响正常的解码，只做参考，非线程安全函数
    //@param pts 要设置的时间 ms
    virtual void setPlayingMs(long long pts);
    virtual void setIsSyn(bool ok);
	void run();
	
protected:
    FQFVideoDevice *vd = nullptr;
    FQFScale *sca = nullptr;
	std::mutex vmux;
    long long synPts = 0;
    long long pts = 0;
    bool isSyn = true;
    unsigned char *rgb = nullptr;
};
}
