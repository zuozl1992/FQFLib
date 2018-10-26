#include "FQF.h"
#include "FQFMedia.h"
#include "configure.h"
#include "infoOutput.h"
#include <QObject>
extern "C"
{
#include <libavformat/avformat.h>
}

FQFMedia::FQFMedia()
{
    FQF::getObject();
    errorbuf[0] = '\0';
    rtspUrl[0] = '\0';
    rtmpUrl[0] = '\0';
    myLock = new mutex;
}

FQFMedia::~FQFMedia()
{
    closeStream();
    delete myLock;
}

bool FQFMedia::openInStream(const char *url)
{
    closeStream();
    myLock->lock();

    AVDictionary *options = NULL;
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    av_dict_set(&options, "buffer_size", "102400", 0); //设置缓存大小，1080p可将值调大
    av_dict_set(&options, "stimeout", "2000000", 0); //设置超时断开连接时间，单位微秒
    av_dict_set(&options, "max_delay", "500000", 0); //设置最大时延

    inFmtCtx = avformat_alloc_context();
    if(!inFmtCtx)
    {
        //失败
        strcpy(errorbuf, "In ctx alloc error!");
        myLock->unlock();
        return false;
    }

    int re;
    //打开视频流
    re = avformat_open_input(&inFmtCtx, url, NULL, &options);
    if ((re != 0) || (!inFmtCtx))
    {
        //失败
        strcpy(errorbuf, "In stream open failed!");
        myLock->unlock();
        return false;
    }
    strcpy(rtspUrl, url);

    re = avformat_find_stream_info(inFmtCtx,NULL);
    if(re < 0)
    {
        av_strerror(re, errorbuf,sizeof(errorbuf));
        myLock->unlock();
        return false;
    }

    myLock->unlock();
    return true;
}

bool FQFMedia::openTestStream(const char *url)
{
    myLock->lock();

    closeTestStream();
    AVDictionary *options = NULL;
    av_dict_set(&options, "stimeout", "1000000", 0); //设置超时断开连接时间，单位微秒
    av_dict_set(&options, "live", "1", 0);
    int re;
    //打开视频流
    re = avformat_open_input(&testFmtCtx, url, NULL, &options);
    if (re != 0)
    {
        //失败
        strcpy(errorbuf, "Test stream open failed!");
        myLock->unlock();
        return false;
    }

    myLock->unlock();
    return true;
}

bool FQFMedia::openOutStream(const char *url)
{
    myLock->lock();

    if(!inFmtCtx)
    {
        strcpy(errorbuf, "In stream not open!");
        myLock->unlock();
        return false;
    }

    int re;

    re = avformat_alloc_output_context2(&outFmtCtx, 0, "flv", url);
    if (!outFmtCtx)
    {
        strcpy(errorbuf, "Out stream open failed!");
        myLock->unlock();
        return false;
    }

    //配置输出流
    //遍历输入的AVStream
    for (quint16 i = 0; i < inFmtCtx->nb_streams; i++)
    {
        //创建输出流
        AVStream *out = avformat_new_stream(outFmtCtx, NULL);
        if (!out)
        {
            strcpy(errorbuf, "stream alloc failed!");
            myLock->unlock();
            return false;
        }
        //复制配置信息,
        re = avcodec_parameters_copy(out->codecpar, inFmtCtx->streams[i]->codecpar);

    }

    //打开IO
    re = avio_open(&outFmtCtx->pb, url, AVIO_FLAG_WRITE);

    if (!outFmtCtx->pb)
    {
        strcpy(errorbuf, "stream io open failed!");
        myLock->unlock();
        return false;
    }
    strcpy(rtmpUrl, url);

    //写入头信息
    re = avformat_write_header(outFmtCtx, 0);
    if (re != AVSTREAM_INIT_IN_WRITE_HEADER)
    {
        av_strerror(re, errorbuf, sizeof(errorbuf));
        myLock->unlock();
        return false;
    }

    bTimeoutState = true;

    myLock->unlock();
    return true;
}

void FQFMedia::closeStream()
{
    myLock->lock();
    bStartFirst = true;
    bTimeoutState = false;
    closeInStream();
    closeOutStream();
    myLock->unlock();
}

bool FQFMedia::readStream()
{
    myLock->lock();

    if (!bTimeoutState || !inFmtCtx)
    {
        strcpy(errorbuf, "stream not open!");

        if (readErrorNum++ > 5)
            bTimeoutState = false;
        myLock->unlock();
        return false;
    }

    if(packet)
        av_packet_free(&packet);
    packet = av_packet_alloc();

    if(!packet)
    {
        strcpy(errorbuf, "packet alloc error!");

        if (readErrorNum++ > 5)
            bTimeoutState = false;
        myLock->unlock();
        return false;
    }

    int re;
    re = av_read_frame(inFmtCtx, packet);
    if (re != 0)
    {
        //失败
        av_strerror(re, errorbuf, sizeof(errorbuf));
        if (readErrorNum++ > 5)
            bTimeoutState = false;
        myLock->unlock();
        return false;
    }

    if(bStartFirst)
    {
        if(packet->flags & AV_PKT_FLAG_KEY )
        {
            bStartFirst = false;
        }
        else
        {
            strcpy(errorbuf, "First packet is not key!");
            myLock->unlock();
            return false;
        }
    }

    readErrorNum = 0;
    myLock->unlock();
    return true;
}

bool FQFMedia::pushPacket()
{
    myLock->lock();

    if (!bTimeoutState || !outFmtCtx || !inFmtCtx)
    {
        strcpy(errorbuf, "Stream not open!");
        myLock->unlock();
        return false;
    }

    if (!packet)
    {
        strcpy(errorbuf, "inStream not read!");
        myLock->unlock();
        return false;
    }

    if (packet->size <= 0)
    {
        strcpy(errorbuf, "stream read failed!");
        myLock->unlock();
        return false;
    }

    AVRational itime = inFmtCtx->streams[packet->stream_index]->time_base;
    AVRational otime = outFmtCtx->streams[packet->stream_index]->time_base;
    packet->pts = av_rescale_q_rnd(packet->pts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_NEAR_INF));
    packet->dts = av_rescale_q_rnd(packet->dts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_NEAR_INF));
    packet->duration = av_rescale_q_rnd(packet->duration, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_NEAR_INF));
    packet->pos = -1;

    int re;
    re = av_interleaved_write_frame(outFmtCtx, packet);
    if (re != 0)
    {
        strcpy(errorbuf, "Packet write failed!");
        if (pushErrorNum++ > 5)
            bTimeoutState = false;
        myLock->unlock();
        return false;
    }
    pushErrorNum = 0;

    myLock->unlock();
    return true;
}

QString FQFMedia::getError()
{
    myLock->lock();
    QString str = this->errorbuf;
    myLock->unlock();
    return str;
}

bool FQFMedia::getTimeoutState()
{
    bool ok = bTimeoutState;
    return ok;
}

void FQFMedia::closeInStream()
{
    if(inFmtCtx)
        avformat_close_input(&inFmtCtx);
    if(packet)
        av_packet_free(&packet);
    rtspUrl[0] = '\0';
    readErrorNum = 0;
}

void FQFMedia::closeOutStream()
{
    if(outFmtCtx)
    {
        avio_close(outFmtCtx->pb);
        avformat_free_context(outFmtCtx);
        outFmtCtx = NULL;
    }
    rtmpUrl[0] = '\0';
    pushErrorNum = 0;
}

void FQFMedia::closeTestStream()
{
    if(testFmtCtx)
        avformat_close_input(&testFmtCtx);
}
