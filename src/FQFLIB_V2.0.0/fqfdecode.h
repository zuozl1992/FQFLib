#pragma once
#include <mutex>
struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;

namespace FQF {
class  FQFDecode
{
public:
    FQFDecode();
    virtual ~FQFDecode();
    //打开解码器，会先执行关闭操作，closeDecode()不管成功与否都将释放AVCodecParameters空间
    //@param 流的编码信息，需要子FFmpeg中获取，用于打开对应的解码器
    //@return 打开结果
    virtual bool openDecode(AVCodecParameters *para);
    //关闭解码器，关闭解码器
    virtual void closeDecode();
    //退出解码器，释放空间
    virtual void exitDecode();
    //清理空间，会释放解码器中的缓冲数据
    virtual void clearDecode();
	//发送到解码线程，不管成功与否都释放pkt空间（对象和媒体内容）
    //@param pkt 要解码的pkt
    //@return bool 发送结果
    virtual bool sendPacketToDecode(AVPacket *pkt);
    //发送nullptr到解码器，当歌曲发送结束应当发送nullptr到解码器以获取缓冲数据
    //@return bool 发送结果，当发送nullptr返回false表明缓冲读取结束
    virtual bool sendNullptrToDecode();
    //获取解码后数据，一次send可能需要多次Recv，
    //获取缓冲中的数据sendNullptrToDecode()再recvFrameFromDecode()多次
    //@return AVFrame *获取到的frame，空间由共享库动态分配，需要由调用者调用FQFFreeFrame()函数释放
    virtual AVFrame * recvFrameFromDecode();
    //获取当前解码的pts
    //@return long long 当前正在解码的pts ms
    virtual long long getDecodingMs();
    virtual void setDecodingMs(long long pts);

protected:
    AVCodecContext *codec = nullptr;
	std::mutex mux;
    long long pts = 0;
};
}
