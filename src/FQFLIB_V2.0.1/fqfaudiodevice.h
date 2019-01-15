#pragma once
namespace FQF {
class FQFAudioDevice
{
public:
    FQFAudioDevice(){}
    virtual ~FQFAudioDevice(){}
    //打开音频设备，该操作会先执行关闭操作，打开的样本大小为16b。
    //@param sampleRate 采样率
    //@param channels 声道数
    //@return bool 操作结果
    virtual bool openDevice(int sampleRate, int channels) = 0;
    //关闭设备，该操作应当关闭设备
    virtual void closeDevice() = 0;
    //退出清理，该操作应当关闭并清理设备，同时释放内存，退出时调用
    virtual void exitDevice() = 0;
    //清理，该操作会释放播放缓冲，不会关闭设备
    virtual void clearDevice() = 0;
    //获取缓冲队列播放时间
    //@return long long 当前缓冲队列还可以播放的时长 ms
    virtual long long getBufferQueueLength() = 0;
    //获取缓冲队列长度，单位字节
    //@return int 当前缓冲队列中还未播放的长度 Byte
    virtual int getBufferLength() = 0;
    //向播放设备写入音频数据，写入数据要求是解码完成的PCM数据，需要与打开设备时配置的采样率、样本数、声道数一致
    //如果声音基带断裂，请检查参数一致性
    //@param data PCM数据
    //@param length PCM数据长度
    //@return bool 操作结果
    virtual bool writeToDeviceBuffer(const unsigned char *data, int length) = 0;
    //暂停
    //@param isPause 为true时暂停播放
    virtual void setPause(bool isPause) = 0;

};

}

