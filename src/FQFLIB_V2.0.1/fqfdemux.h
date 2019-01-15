#pragma once
#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

namespace FQF {

class FQFDemux
{
public:
    //pkt的格式
    enum PacketType{
        AudioPacket,    //音频数据
        VideoPacket,
        OtherPacket,    //其他数据
        NullPacket      //空
    };
    enum MediaType{
        VideoMedia,
        VideoMedia_NoAudio,
        AudioMedia,
        AudioMedia_NoVideo,
        Stream,
        Stream_NoAudio,
        Stream_NoVideo,
        OtherMedia
    };
    enum OpenType{
        VideoFile,
        AudioFile,
        VideoStream,
        AudioStream
    };
    FQFDemux();
    virtual ~FQFDemux();
    //打开媒体文件，打开前会执行关闭操作
    //@param path 要打开的文件的路径
    //@return bool 打开结果
    virtual bool openDemux(const char *path,OpenType openType);
    //关闭文件，会关闭输入流，同时会将视频参数恢复初始值
    virtual void closeDemux();
    //退出文件，会退出输入流，同时会将视频参数恢复初始值
    virtual void exitDemux();
    //清理空间，清理读取缓冲
    virtual void clearDemux();
    //读取文件
    //@return AVPacket * 读取到的内容，空间需由调用者释放，包括AVPacket对象空间，和数据空间
    //释放参见FQFFreePacket()
    virtual AVPacket *readFile();
    //获取pkt的类型，不会清理pkt空间
    //@param pkt 需要判断类型的pkt
    //@return PacketType pkt类型
    virtual PacketType getPktType(AVPacket *pkt);
    //复制得到Audio的媒体参数，
    //@return AVCodecParameters * 媒体参数，空间需要在外部清理 avcodec_parameters_free()
    virtual AVCodecParameters *copyAudioPara();
    //复制得到video的媒体参数，
    //@return AVCodecParameters * 媒体参数，空间需要在外部清理 avcodec_parameters_free()
    virtual AVCodecParameters *copyVideoPara();
    //媒体跳转
    //@param pos 位置 0.0~1.0
    //@return bool 跳转结果
    virtual bool seek(double pos);
    //获取文件的媒体时长
    //@return long long 当前媒体时长 ms
    virtual long long getFileTimeMs();
    //获取当前打开文件的采样率
    //@return int 采样率
    virtual int getFileSampleRate();
    //获取当前打开文件的视频的宽
    //@return int 宽度
    virtual int getFileWidth();
    //获取当前打开文件的视频的高
    //@return int 高度
    virtual int getFileHeight();
    //获取当前打开文件的声道数
    //@return int 声道
    virtual int getFileChannels();
    //获取当前打开文件的类型
    //@return MediaType 类型
    virtual MediaType getMediaType();
    //获取音乐文件的专辑封面
    //@param data 数据
    //@param size 空间大小
    //@return bool 结果
    virtual bool getMusicTitlePage(unsigned char **data,int *size);
    //释放共享库中分配的空间
    //@param buf 要释放的空间
    virtual void freeBuffer(unsigned char **buf);

protected:
    AVFormatContext *ic = nullptr;
	std::mutex mux;
    int audioStream = -1;
    int videoStream = -1;
    long long totalMs = 0;
    int sampleRate = 0;
    int channels = 0;
    int width = 0;
    int height = 0;
    MediaType mediaType = OtherMedia;
};

}
