#pragma once
#include <QThread>
#include <mutex>

namespace FQF {
class FQFDemux;
class FQFAudioThread;
class FQFVideoThread;
class FQFAudioDevice;
class FQFVideoDevice;

class FQFDemuxThread : public QThread
{
    Q_OBJECT
public:
    //文件播放状态
    enum MusicType{
        Play,   //播放
        Stop,   //停止
        Pause,  //暂停
        End     //结束
    };
    //构造函数
    //@param vcall 视频播放组件，如没有视频，只需要传入null即可
    //@param acall 音频播放组件，如没有音频，只需要传入null即可
    FQFDemuxThread(FQFVideoDevice *vcall = nullptr,FQFAudioDevice *acall = nullptr);
    virtual ~FQFDemuxThread();
    //设置视频回调指针
    //@param call 视频播放设备指针
    virtual void setVideoDeviceCallBack(FQFVideoDevice *call = nullptr);
    //设置音频回调指针
    //@param call 音频播放设备指针
    virtual void setAudioDeviceCallBack(FQFAudioDevice *call = nullptr);
    //打开文件，该函数为顶层操作，函数内部会执行上下文打开、解码器打开、播放打开等一系列操作
    //@param path 要打开文件的路径
    //@return bool 打开结果
    virtual bool openFileTh(const char *path,int type);
    //关闭文件，该函数为顶层操作，函数内部会关闭上下文，关闭解码器，退出当前线程，退出解码线程
    virtual void closeFileTh();
    //退出文件，该函数为顶层操作，函数内部会关闭上下文，退出解码器，退出当前线程，退出解码线程
    virtual void exitFileTh();
    //清理，该函数为顶层操作，同时会清理解码所有涉及对象，只是清理缓冲不影响继续播放
    virtual void clearFileTh();
    //暂停/播放
    //@param isPause 是否暂停
    virtual void setPause(bool isPause);
    //媒体跳转
    //@param pos 位置 0.0~1.0
    //@return bool 跳转结果
    virtual void seek(double pos);
    //获取当前媒体的播放状态
    //@return MusicType 状态
    virtual MusicType getMusicStatus();
    //获取当前正在播放的进度
    //@return long long 当前的时间 ms
    virtual long long getNowTimeMs();
    //获取当前文件的总时长
    //@return long long 媒体总时长 ms
    virtual long long getFileTimeMs();
    //获取视频的宽，如果没有视频流，则返回0
    //@return int 视频宽度
    virtual int getVideoWidth();
    //获取视频的高，如果没有视频流，则返回0
    //@return int 视频高度
    virtual int getVideoHeight();
	void run();

protected:
    virtual void startTh();
    FQF::FQFDemux *demux = nullptr;
    FQFAudioThread *at = nullptr;
    FQFVideoThread *vt = nullptr;
	std::mutex mux;
	bool isExit = false;
    MusicType status = Stop;
    long long pts = 0;
    long long totalMs = 0;
};
}
