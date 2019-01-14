#include "fqfresample.h"
extern "C"
{
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}
using namespace FQF;
FQFResample::FQFResample(){}

FQFResample::~FQFResample(){}

bool FQFResample::openResample(AVCodecParameters * para, bool isClearPara)
{
	if (!para)return false;
    closeResample();
	mux.lock();
	//音频重采样 上下文初始化
	//如果actx为NULL，会自动分配空间
    actx = swr_alloc_set_opts(actx,
                              av_get_default_channel_layout(para->channels),	//输出格式
                              static_cast<AVSampleFormat>(outFormat),			//输出样本格式 1 AV_SAMPLE_FMT_S16
                              para->sample_rate,                                //输出采样率
                              av_get_default_channel_layout(para->channels),    //输入格式
                              static_cast<AVSampleFormat>(para->format),
                              para->sample_rate,
                              0, nullptr
                              );
	if(isClearPara)
		avcodec_parameters_free(&para);
	int re = swr_init(actx);
	mux.unlock();
	if (re != 0)
	{
		
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		return false;
	}
	//unsigned char *pcm = NULL;
	return true;
}

void FQFResample::closeResample()
{
	mux.lock();
	if (actx)
		swr_free(&actx);
    mux.unlock();
}

void FQFResample::exitResample()
{
    mux.lock();
    if (actx)
        swr_free(&actx);
    mux.unlock();
}

int FQFResample::resample(AVFrame * inData, unsigned char * outData)
{
    if (!inData)return 0;
    if (!outData)
	{
        av_frame_free(&inData);
		return 0;
	}
    if(!actx)return 0;
    uint8_t *data[2] = { nullptr };
    data[0] = outData;
	int re = swr_convert(actx,
        data, inData->nb_samples,                                       //输出
        const_cast<const uint8_t**>(inData->data), inData->nb_samples	//输入
	);
    int outSize = re * inData->channels * av_get_bytes_per_sample(static_cast<AVSampleFormat>(outFormat));
    av_frame_free(&inData);
	if (re <= 0)return re;
	return outSize;
}
