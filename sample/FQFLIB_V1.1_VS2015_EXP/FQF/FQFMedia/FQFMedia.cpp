#include "FQFMedia.h"
#include "FQF/FQFCore/FQFCore.h"

FQFMedia::FQFMedia()
{
    FQFCore::getObject();
	errorbuf[0] = '\0';
}

FQFMedia::~FQFMedia()
{
}
static int CheckInterrupt(void* ctx)
{
    auto p = (FQFMedia *)ctx;
	int timeout = p->isReadFrame() ? p->getReadFrameTimeout() : p->getOpenInputTimeout();
	if (time(NULL) - p->getStartTime() >= timeout)
	{
		p->setTimeoutState(false);
		if (p->isReadFrame())
			p->reconnect();
		return 1;
	}
	else
	{
		return 0;
	}
}

bool FQFMedia::openStream(const char * url)
{
	closeStream();
	mutex.lock();
	
	AVDictionary *options = NULL;
    av_dict_set(&options, "rtsp_transport", rtspTransport ? "tcp" : "udp", 0);

	iCtx = avformat_alloc_context();
	iCtx->interrupt_callback.callback = CheckInterrupt;//超时回调
	iCtx->interrupt_callback.opaque = this;

	bIsReadFrame = false;
	startTime = time(NULL);
    int re;
    re = avformat_open_input(&iCtx, url, NULL, &options);
	if (re != 0)
	{
		av_strerror(re, errorbuf, sizeof(errorbuf));
		mutex.unlock();
		return false;
    }
	strcpy(rtspUrl, url);
	for (int i = 0; i < iCtx->nb_streams; i++)
	{
		if (iCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStream = i;
			AVCodec *codec = avcodec_find_decoder(iCtx->streams[i]->codec->codec_id);
			if (!codec)
			{
				strcpy(errorbuf, "stream video code not find!");
				mutex.unlock();
				return false;
			}
			re = avcodec_open2(iCtx->streams[i]->codec, codec, NULL);
			if (re != 0)
			{
				av_strerror(re, errorbuf, sizeof(errorbuf));
				mutex.unlock();
				return false;
			}
		}
	}
	timeoutState = true;
	reconnectionNum = 0;
	mutex.unlock();
	return true;
}

void FQFMedia::closeStream()
{
	mutex.lock();
	timeoutState = false;
	if (iCtx)
		avformat_close_input(&iCtx);
	if (yuv)
		av_frame_free(&yuv);
	if (vCtx)
	{
		sws_freeContext(vCtx);
		vCtx = NULL;
	}
	mutex.unlock();
}

AVPacket FQFMedia::readStream()
{
	AVPacket pkt;
	memset(&pkt, 0, sizeof(AVPacket));	//清空
	mutex.lock();
	if (!iCtx)
	{
		strcpy(errorbuf, "stream not open!");
		mutex.unlock();
		return pkt;
	}

	bIsReadFrame = true;
	startTime = time(NULL);

	int re = av_read_frame(iCtx, &pkt);
	if (re != 0)
	{
		av_strerror(re, errorbuf, sizeof(errorbuf));
	}
    
	mutex.unlock();
	return pkt;
}

bool FQFMedia::decodePacket(const AVPacket * pkt)
{
	mutex.lock();
	if (!iCtx)
	{
		strcpy(errorbuf, "stream not open!");
		mutex.unlock();
		return false;
	}
	if (yuv == NULL)
	{
		yuv = av_frame_alloc();
	}
	int re = avcodec_send_packet(iCtx->streams[pkt->stream_index]->codec, pkt);
	if (re != 0)
	{
		av_strerror(re, errorbuf, sizeof(errorbuf));
		mutex.unlock();
		return false;
	}
	re = avcodec_receive_frame(iCtx->streams[pkt->stream_index]->codec, yuv);
	if (re != 0)
	{
		av_strerror(re, errorbuf, sizeof(errorbuf));
		mutex.unlock();
		return false;
	}
	if (pkt->stream_index == videoStream)
	{
		yuvWidth = yuv->width;
		yuvHeight = yuv->height;
	}
	mutex.unlock();
	return true;
}

bool FQFMedia::yuvToRgb(char * outSpace, int outWidth, int outHeight)
{
	mutex.lock();
	if (!iCtx || !yuv)
	{
		strcpy(errorbuf, "stream not open or yuv not decoded!");
		mutex.unlock();
		return false;
	}
	AVCodecContext * videoCtx = iCtx->streams[this->videoStream]->codec;
	vCtx = sws_getCachedContext(vCtx,
		videoCtx->width, videoCtx->height, videoCtx->pix_fmt,
		outWidth, outHeight, AV_PIX_FMT_BGRA,
		SWS_BICUBIC, NULL, NULL, NULL
	);
	if (!vCtx)
	{
		strcpy(errorbuf, "sws getCachedContext failed!");
		mutex.unlock();
		return false;
	}
	uint8_t *data[AV_NUM_DATA_POINTERS] = { 0 };
	data[0] = (uint8_t *)outSpace;
	int linesize[AV_NUM_DATA_POINTERS] = { 0 };
	linesize[0] = outWidth * 4;
	int h = sws_scale(vCtx, yuv->data, yuv->linesize, 0, videoCtx->height,
		data, linesize);
	if (h <= 0)
	{
		strcpy(errorbuf, "sws scale failed!");
		mutex.unlock();
		return false;
	}
	mutex.unlock();
	return true;
}

std::string FQFMedia::getError()
{
	mutex.lock();
	std::string str = this->errorbuf;
	mutex.unlock();
	return str;
}

bool FQFMedia::packetIsVideo(int streamIndex)
{
	if (videoStream == streamIndex)
		return true;
	return false;
}

bool FQFMedia::getYuvSize(int * w, int * h)
{
	if (yuvWidth == 0 || yuvHeight == 0)
		return false;
	*w = yuvWidth;
	*h = yuvHeight;
    return true;
}

time_t FQFMedia::getStartTime()
{
    return startTime;
}

void FQFMedia::setRtspTransport(bool state)
{
    rtspTransport = state;
}

bool FQFMedia::getTimeoutState()
{
	return timeoutState;
}

void FQFMedia::setTimeoutState(bool state)
{
	timeoutState = state;
}

void FQFMedia::setOpenInputTimeout(int time)
{
	openInputTimeout = time;
}

int FQFMedia::getOpenInputTimeout()
{
	return openInputTimeout;
}

void FQFMedia::setReadFrameTimeout(int time)
{
	readFrameTimeout = time;
}

int FQFMedia::getReadFrameTimeout()
{
	return readFrameTimeout;
}

bool FQFMedia::isReadFrame()
{
	return bIsReadFrame;
}

bool FQFMedia::reconnect()
{
	if (reconnectionNum++ >= 3)
		return false;
	bool ok = this->openStream(rtspUrl);
	if (ok)
	{
		reconnectionNum = 0;
		return true;
	}
	return false;
}
