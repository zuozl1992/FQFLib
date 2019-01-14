#pragma once
#include <QThread>
#include <mutex>
#include <list>

struct AVPacket;
namespace FQF {
class FQFDecode;
class FQFDecodeThread : public QThread
{
public:
    //解码线程状态
    enum DecodeType{
        Play,   //播放
        Stop,   //停止
        Pause,  //暂停
        End     //读取完毕
    };
	FQFDecodeThread();
	virtual ~FQFDecodeThread();
    //将pkt添加到缓冲队列尾部，该操作会阻塞插入，如果缓冲队列溢出则会等待
    //@param pkt 读取到的pkt
    virtual void pushPacketToBufferList(AVPacket *pkt);
    //从缓冲队列中读取pkt，如果队列为空则返回nullptr
    //@return AVPacket * 队首的pkt或nullptr
    virtual AVPacket *popPacketFromBufferList();
    //关闭函数，会清理缓冲、释放空间、关闭解码器、释放解码器空间
    virtual void closeDecodeTh();
    //退出函数，会清理缓冲、释放空间、退出解码器、释放解码器空间、停止线程
    virtual void exitDecodeTh();
    //清理函数，会清空缓冲队列、释放缓冲空间
    virtual void clearDecodeTh();
    //设置解码线程状态
    //@param s 状态
    virtual void setDecodeType(DecodeType s);
    //获取当前的解码状态
    //@return DecodeType 当前的解码状态
    virtual DecodeType getDecodeType();
    //设置读取结束标志，当文件结束则应该调用该函数告诉解码线程，解码线程会通过发送nullptr的方式
    //将解码器中缓冲的数据读取出来
    virtual void setFinishedReading();

protected:
	std::list <AVPacket *> packs;
    FQFDecode *decode = nullptr;
    std::mutex mux;
    DecodeType status = Stop;
    bool isFinishedReading = false;
    unsigned int maxList = 1000;
    bool isExit = false;
    bool openSu = false;
};
}
