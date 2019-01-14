#pragma once
#include "fqfdecodethread.h"
#include <mutex>
#include <list>

struct AVCodecParameters;
namespace FQF {
class FQFAudioDevice;
class FQFResample;
class FQFAudioThread : public FQFDecodeThread
{
public:
    FQFAudioThread();
    virtual ~FQFAudioThread();
    //设置音频播放设备指针，需要再打开前传入
    virtual void setCallBack(FQFAudioDevice *call);
    //打开解码器线程，不管成功与否都将清理para
    //@param para 音频的编码信息，需要子FFmpeg中获取，用于打开对应的解码器
    //@param sampleRate 采样率，需要与para中的一致，此处不会校验
    //@param channels 声道数，需要与para中的一致，否则可能造成程序崩溃，此处不会校验
    //@return bool 打开结果，成功返回true
    virtual bool openDecodeTh(AVCodecParameters *para, int sampleRate, int channels);
    //关闭解码器，会先调用基类的关闭函数
    //基类的关闭函数会清理缓冲、关闭解码器
    //该函数会清理并释放重采样成员，同时关闭播放设备
    virtual void closeDecodeTh();
    //退出解码器，会先调用基类的退出函数，退出时调用
    //基类的退出函数会清理缓冲、释放空间、退出解码器、释放解码器空间、停止线程
    //该函数会清理并释放重采样成员，同时退出播放设备
    virtual void exitDecodeTh();
    //清理函数，会先调用基类的清理函数
    //基类的清理函数会清空缓冲、释放缓冲空间
    //该函数会释放播放设备的播放缓冲
    virtual void clearDecodeTh();
    //暂停函数，非线程安全函数
    //@param isPause 是否暂停
    virtual void setPause(bool isPause);
    //获取实际正在播放的数据的pts
    //@return long long 当前正在播放的pts ms
    virtual long long getPlayingMs();
    //设置播放pts，用于seek后同步音频时间，不会影响正常的解码，只做参考，非线程安全函数
    //@param pts 要设置的时间 ms
    virtual void setPlayingMs(long long pts);
    virtual void clearPcmBuffer();
    //线程运行函数入口
	void run();

protected:
    FQF::FQFAudioDevice *ad = nullptr;
    FQFResample *res = nullptr;
	std::mutex amux;
    long long pts = 0;
    unsigned char *pcm = nullptr;
};
}
