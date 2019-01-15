#include "fqfdemux.h"
#include <QDebug>
extern "C" {
	#include "libavformat/avformat.h"
}
using namespace FQF;
static double r2d(AVRational r)
{
    return r.den == 0 ? 0 : static_cast<double>(r.num)/ static_cast<double>(r.den);
}

FQFDemux::FQFDemux(){}
FQFDemux::~FQFDemux(){}

bool FQFDemux::openDemux(const char * path, OpenType openType)
{
    closeDemux();
	mux.lock();
    AVDictionary * options = nullptr;
    if(openType >= 2)
    {
        av_dict_set(&options, "buffer_size", "1024000", 0);
        av_dict_set(&options, "stimeout", "20000000", 0);
        av_dict_set(&options, "max_delay", "500000", 0);
        av_dict_set(&options, "rtsp_transport", "tcp", 0);
    }
	//解封装上下文
	int re = avformat_open_input(
		&ic,
        path,
        nullptr,  // 0表示自动选择解封器
        &options   //参数设置，比如rtsp的延时时间
	);
	if (re != 0)
	{
		mux.unlock();
		return false;
	}

	//获取流信息 
    re = avformat_find_stream_info(ic, nullptr);

	//总时长 毫秒
	totalMs = ic->duration / (AV_TIME_BASE / 1000);

	//获取音频流
    audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

    switch (openType) {
    case VideoFile:
        if(videoStream < 0)
        {
            mux.unlock();
            return false;
        }
        if(audioStream < 0)
        {
            mediaType = VideoMedia_NoAudio;
        }
        else {
            mediaType = VideoMedia;
        }
        break;
    case AudioFile:
        if(audioStream < 0)
        {
            mux.unlock();
            return false;
        }
        if(videoStream < 0)
        {
            mediaType = AudioMedia_NoVideo;
        }
        else {
            mediaType = AudioMedia;
            //专辑处理

            videoStream = -1;
        }
        break;
    case VideoStream:
        if(videoStream < 0)
        {
            mux.unlock();
            return false;
        }
        if(audioStream < 0)
        {
            mediaType = Stream_NoAudio;
        }
        else {
            mediaType = Stream;
        }
        break;
    case AudioStream:
        if(audioStream < 0)
        {
            mux.unlock();
            return false;
        }
        if(videoStream < 0)
        {
            mediaType = Stream_NoVideo;
        }
        else {
            mediaType = Stream;
        }
        break;
    }
    AVStream *stream;

    if(audioStream >= 0)
    {
        stream = ic->streams[audioStream];
        sampleRate = stream->codecpar->sample_rate;
        channels = stream->codecpar->channels;
    }
    if(videoStream >= 0)
    {
        stream = ic->streams[videoStream];
        width = stream->codecpar->width;
        height = stream->codecpar->height;
    }
	mux.unlock();
	return true;
}



void FQFDemux::closeDemux()
{
    mux.lock();
    if (ic)
        avformat_close_input(&ic);
    totalMs = 0;
    audioStream = -1;
    videoStream = -1;
    sampleRate = 0;
    channels = 0;
    width = 0;
    height = 0;
    mediaType = FQFDemux::OtherMedia;
    mux.unlock();
}

void FQFDemux::exitDemux()
{
    mux.lock();
    if (ic)
        avformat_close_input(&ic);
    totalMs = 0;
    audioStream = -1;
    videoStream = -1;
    sampleRate = 0;
    channels = 0;
    width = 0;
    height = 0;
    mediaType = FQFDemux::OtherMedia;
    mux.unlock();
}

void FQFDemux::clearDemux()
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return;
    }
    avformat_flush(ic);
    mux.unlock();
}

AVPacket * FQFDemux::readFile()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
        return nullptr;
	}
	AVPacket *packet = av_packet_alloc();
	//读取一帧，并分配空间
	int re = av_read_frame(ic, packet);
	if (re != 0)
	{
		mux.unlock();
		av_packet_free(&packet);
        return nullptr;
	}
	//pts转换为毫秒
    packet->pts = static_cast<long long>( packet->pts * (1000.0 * r2d(ic->streams[packet->stream_index]->time_base) ));
    packet->dts = static_cast<long long>( packet->dts * (1000.0 * r2d(ic->streams[packet->stream_index]->time_base) ));
    mux.unlock();
    return packet;
}

FQFDemux::PacketType FQFDemux::getPktType(AVPacket * pkt)
{
	if (!pkt)return NullPacket;
	if (pkt->stream_index == audioStream)
		return AudioPacket;
    if (pkt->stream_index == videoStream)
        return VideoPacket;
	return OtherPacket;
}

AVCodecParameters * FQFDemux::copyAudioPara()
{
	mux.lock();
    if (!ic || audioStream < 0)
	{
		mux.unlock();
        return nullptr;
	}
	AVCodecParameters *pa = avcodec_parameters_alloc();
	int re = avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);
	if (re < 0)
	{
		avcodec_parameters_free(&pa);
        return nullptr;
	}
	mux.unlock();
    return pa;
}

AVCodecParameters *FQFDemux::copyVideoPara()
{
    mux.lock();
    if (!ic || videoStream < 0)
    {
        mux.unlock();
        return nullptr;
    }
    AVCodecParameters *pa = avcodec_parameters_alloc();
    int re = avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);
    if (re < 0)
    {
        avcodec_parameters_free(&pa);
        return nullptr;
    }
    mux.unlock();
    return pa;
}

bool FQFDemux::seek(double pos)
{
    mux.lock();
    if (!ic)
    {
        mux.unlock();
        return false;
    }

    int seekStream;
    if(mediaType == FQFDemux::VideoMedia_NoAudio || mediaType == FQFDemux::Stream_NoAudio)
    {
        seekStream = videoStream;
    }
    else {
        seekStream = audioStream;
    }
    if(seekStream == -1)
    {
        mux.unlock();
        return false;
    }
    long long seekPos = 0;
    seekPos = static_cast<long long>((static_cast<double>(ic->streams[seekStream]->duration) * pos));

    int re = av_seek_frame(ic, seekStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    mux.unlock();
    return re < 0 ? false : true;
}

long long FQFDemux::getFileTimeMs()
{
    return totalMs;
}

int FQFDemux::getFileSampleRate()
{
    return sampleRate;
}

int FQFDemux::getFileWidth()
{
    return width;
}

int FQFDemux::getFileHeight()
{
    return height;
}

int FQFDemux::getFileChannels()
{
    return channels;
}

FQFDemux::MediaType FQFDemux::getMediaType()
{
    return mediaType;
}

bool FQFDemux::getMusicTitlePage(unsigned char **data, int *size)
{
    if(!ic)
        return false;
    if(mediaType != AudioMedia)
        return false;
    for(unsigned int i = 0; i < ic->nb_streams; i++)
    {
        if(ic->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC)
        {
            *size = ic->streams[i]->attached_pic.size;
            (*data) = new unsigned char[static_cast<unsigned long long>(*size)];
            memcpy((*data),ic->streams[i]->attached_pic.data,static_cast<unsigned long long>(*size));
            return true;
        }
    }
    return false;
}

void FQFDemux::freeBuffer(unsigned char **buf)
{
    delete [] (*buf);
    (*buf) = nullptr;
}
