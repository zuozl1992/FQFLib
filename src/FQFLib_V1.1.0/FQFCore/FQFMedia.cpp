#include "FQFMedia.h"
#include "FQF.h"

FQFMedia::FQFMedia()
{
	FQF::getObject();
	errorbuf[0] = '\0';
	locker = new QMutex;
}

FQFMedia::~FQFMedia()
{

}
//超时回掉函数
static int CheckInterrupt(void* pointer)
{
    auto p = (FQFMedia *)pointer;
	int timeout = p->isReadFrame() ? p->getReadFrameTimeout() : p->getOpenInputTimeout();
	if (time(NULL) - p->getStartTime() >= timeout)
	{
		p->setTimeoutState(false);
		return 1;
	}
	else
		return 0;
}

bool FQFMedia::openStream(const char * url)
{
	closeStream();
	locker->lock();
	
	AVDictionary *options = NULL;
    av_dict_set(&options, "rtsp_transport", bRtspTransport ? "tcp" : "udp", 0);

	inFmtCtx = avformat_alloc_context();
	inFmtCtx->interrupt_callback.callback = CheckInterrupt;//超时回调
	inFmtCtx->interrupt_callback.opaque = this;

	bIsReadFrame = false;
	startTime = time(NULL);
    int re;
	//打开视频流
    re = avformat_open_input(&inFmtCtx, url, NULL, &options);
	if (re != 0)
	{
		//失败
		av_strerror(re, errorbuf, sizeof(errorbuf));
		locker->unlock();
		return false;
    }
	strcpy(rtspUrl, url);
	//遍历视频中的流信息，保存视频流信息
	for (int i = 0; i < inFmtCtx->nb_streams; i++)
	{
		//视频流
		if (inFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			iVideoStream = i;
			//查找视频解码器
			AVCodec *codec = avcodec_find_decoder(inFmtCtx->streams[i]->codec->codec_id);
			if (!codec)
			{
				strcpy(errorbuf, "stream video code not find!");
				locker->unlock();
				return false;
			}
			//打开视频解码器
			re = avcodec_open2(inFmtCtx->streams[i]->codec, codec, NULL);
			if (re != 0)
			{
				av_strerror(re, errorbuf, sizeof(errorbuf));
				locker->unlock();
				return false;
			}
		}
	}
	bTimeoutState = true;
	locker->unlock();
	return true;
}

void FQFMedia::closeStream()
{
	locker->lock();
	bTimeoutState = false;
	if (inFmtCtx)
		avformat_close_input(&inFmtCtx);
	if (videoFrame)
		av_frame_free(&videoFrame);
	if (videoSwsCtx)
	{
		sws_freeContext(videoSwsCtx);
		videoSwsCtx = NULL;
	}
	locker->unlock();
}

AVPacket FQFMedia::readStream()
{
	AVPacket pkt;
	memset(&pkt, 0, sizeof(AVPacket));	//清空
	locker->lock();
	if (!inFmtCtx)
	{
		strcpy(errorbuf, "stream not open!");
		
		if (readErrorNum++ > 10)
			bTimeoutState = false;
		locker->unlock();
		return pkt;
	}

	bIsReadFrame = true;
	startTime = time(NULL);

	//读取
	int re = av_read_frame(inFmtCtx, &pkt);
	if (re != 0)
	{
		//失败
		av_strerror(re, errorbuf, sizeof(errorbuf));
		if (readErrorNum++ > 10)
			bTimeoutState = false;
		locker->unlock();
		return pkt;
	}
	readErrorNum = 0;
	locker->unlock();
	return pkt;
}

void FQFMedia::freePacket(AVPacket * pkt)
{
	av_packet_unref(pkt);
}

bool FQFMedia::decodePacket(const AVPacket * pkt)
{
	locker->lock();
	if (!inFmtCtx)
	{
		strcpy(errorbuf, "stream not open!");
		if (decodeErrorNum++ > 10)
			bTimeoutState = false;
		locker->unlock();
		return false;
	}
	if (videoFrame == NULL)
	{
		videoFrame = av_frame_alloc();
	}
	//解码，新版本
	//发送pkt
	int re = avcodec_send_packet(inFmtCtx->streams[pkt->stream_index]->codec, pkt);
	if (re != 0)
	{
		av_strerror(re, errorbuf, sizeof(errorbuf));
		if (decodeErrorNum++ > 10)
			bTimeoutState = false;
		locker->unlock();
		return false;
	}
	//获取frame
	re = avcodec_receive_frame(inFmtCtx->streams[pkt->stream_index]->codec, videoFrame);
	if (re != 0)
	{
		av_strerror(re, errorbuf, sizeof(errorbuf));
		if (decodeErrorNum++ > 10)
			bTimeoutState = false;
		locker->unlock();
		return false;
	}
	if (pkt->stream_index == iVideoStream)
	{
		iWidth = videoFrame->width;
		iHeight = videoFrame->height;
	}
	decodeErrorNum = 0;
	locker->unlock();
	return true;
}

bool FQFMedia::yuvToRgb(char * outSpace, int outWidth, int outHeight)
{
	locker->lock();
	if (!inFmtCtx || !videoFrame)
	{
		strcpy(errorbuf, "stream not open or videoFrame not decoded!");
		locker->unlock();
		return false;
	}
	//设置转码器
	AVCodecContext * videoCtx = inFmtCtx->streams[this->iVideoStream]->codec;
	videoSwsCtx = sws_getCachedContext(videoSwsCtx,
		videoCtx->width, videoCtx->height, videoCtx->pix_fmt,
		outWidth, outHeight, AV_PIX_FMT_BGRA,
		SWS_BICUBIC, NULL, NULL, NULL
	);
	if (!videoSwsCtx)
	{
		//失败
		strcpy(errorbuf, "sws getCachedContext failed!");
		if (swsErrorNum++ > 10)
			bTimeoutState = false;
		locker->unlock();
		return false;
	}
	//转码输出空间
	uint8_t *data[AV_NUM_DATA_POINTERS] = { 0 };
	data[0] = (uint8_t *)outSpace;
	//图像宽度
	int linesize[AV_NUM_DATA_POINTERS] = { 0 };
	linesize[0] = outWidth * 4;
	//转码，成功后返回转码图像的高
	int h = sws_scale(videoSwsCtx, videoFrame->data, videoFrame->linesize, 0, videoCtx->height,
		data, linesize);
	if (h <= 0)
	{
		//失败
		strcpy(errorbuf, "sws scale failed!");
		if (swsErrorNum++ > 10)
			bTimeoutState = false;
		locker->unlock();
		return false;
	}
	swsErrorNum = 0;
	locker->unlock();
	return true;
}

std::string FQFMedia::getError()
{
	locker->lock();
	std::string str = this->errorbuf;
	locker->unlock();
	return str;
}

bool FQFMedia::packetIsVideo(int streamIndex)
{
	if (iVideoStream == streamIndex)
		return true;
	return false;
}

bool FQFMedia::getYuvSize(int * w, int * h)
{
	if (iWidth == 0 || iHeight == 0)
		return false;
	*w = iWidth;
	*h = iHeight;
    return true;
}

time_t FQFMedia::getStartTime()
{
    return startTime;
}

void FQFMedia::setRtspTransport(bool state)
{
    bRtspTransport = state;
}

bool FQFMedia::getTimeoutState()
{
	return bTimeoutState;
}

void FQFMedia::setTimeoutState(bool state)
{
	bTimeoutState = state;
}

void FQFMedia::setOpenInputTimeout(int time)
{
	iOpenInputTimeout = time;
}

int FQFMedia::getOpenInputTimeout()
{
	return iOpenInputTimeout;
}

void FQFMedia::setReadFrameTimeout(int time)
{
	iReadFrameTimeout = time;
}

int FQFMedia::getReadFrameTimeout()
{
	return iReadFrameTimeout;
}

bool FQFMedia::isReadFrame()
{
	return bIsReadFrame;
}
