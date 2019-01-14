#include "fqfscale.h"
extern "C"
{
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
}
using namespace FQF;
FQFScale::FQFScale(){}

FQFScale::~FQFScale(){}

bool FQFScale::openScale(AVCodecParameters * para, bool isClearPara)
{
	if (!para)return false;
    closeScale();
	mux.lock();
    if(buf)
    {
        delete [] buf;
    }
    buf = new unsigned char[
            static_cast<unsigned long long>(para->width * para->height * 4)];
    //视频格式转换 上下文初始化
    //如果vctx为NULL，会自动分配空间
    if(para->format == outFormat)
    {
        sws_freeContext(vctx);
        vctx = nullptr;
        mux.unlock();
        return true;
    }
    vctx = sws_getCachedContext(vctx,
                                para->width,
                                para->height,
                                static_cast<AVPixelFormat>(para->format),			//输出样本格式 1 AV_SAMPLE_FMT_S16
                                para->width,
                                para->height,
                                static_cast<AVPixelFormat>(outFormat),
                                SWS_BICUBIC,nullptr,nullptr,nullptr
                              );
	if(isClearPara)
		avcodec_parameters_free(&para);
	mux.unlock();
    if (vctx == nullptr)
	{
		return false;
	}
	return true;
}

void FQFScale::closeScale()
{
	mux.lock();
    if (vctx)
    {
        sws_freeContext(vctx);
        vctx = nullptr;
    }
    mux.unlock();
}

void FQFScale::exitScale()
{
    mux.lock();
    if (vctx)
    {
        sws_freeContext(vctx);
        vctx = nullptr;
    }
    mux.unlock();
}

int FQFScale::scale(AVFrame * inData, AVFrame **outData)
{
    if (!inData)return 0;
    mux.lock();
    if(*outData)
        av_frame_free(outData);
    if(!vctx)
    {
        *outData = inData;
        mux.unlock();
        return inData->height;
    }
    (*outData) = av_frame_alloc();
    (*outData)->format = outFormat;
    (*outData)->width = inData->width;
    (*outData)->height = inData->height;
    int re = av_frame_get_buffer(*outData, 0);
    if (re < 0)
    {
        mux.unlock();
        return 0;
    }
    re = sws_scale(vctx,
                       inData->data,
                       inData->linesize,
                       0,
                       inData->height,
                       (*outData)->data,
                       (*outData)->linesize
	);
    (*outData)->pts = inData->pts;
    av_frame_free(&inData);
    mux.unlock();
    return re;
}

int FQFScale::scale(AVFrame *inData, unsigned char *outData, int *width, int *height)
{
    if (!inData)return 0;
    mux.lock();
    if(!vctx)
    {
        mux.unlock();
        return 0;
    }
    int re;
    *width = inData->width;
    *height = inData->height;
    uint8_t *data[AV_NUM_DATA_POINTERS] = { nullptr };
    data[0] = outData;
    int linesize[AV_NUM_DATA_POINTERS] = { 0 };
    linesize[0] = *width * 4;
    re = sws_scale(vctx,
                       inData->data,
                       inData->linesize,
                       0,
                       inData->height,
                       data,
                       linesize
    );
    av_frame_free(&inData);
    mux.unlock();
    return re;
}
