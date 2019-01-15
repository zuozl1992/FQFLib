#pragma once
#include <mutex>
struct AVCodecParameters;
struct SwrContext;
struct AVFrame;
namespace FQF {
class FQFResample
{
public:
    FQFResample();
    virtual ~FQFResample();
    //打开转码器，输出参数与输入参数一致，除了采样格式，输出采样格式为s16
    //@param para 媒体格式
    //@param isClearPara 是否要清理传入的para，为true时清理
    //@return bool 打开结果
    virtual bool openResample(AVCodecParameters *para,bool isClearPara = false);
    //关闭转码器，释放空间
    virtual void closeResample();
    //退出转码器，释放空间
    virtual void exitResample();
    //进行重采样，不管成功与否都释放indata
    //@param inData 输入数据
    //@param outData 输出数据
    //@return int 重采样后大小
    virtual int resample(AVFrame *inData, unsigned char *outData);

protected:
	std::mutex mux;
    SwrContext *actx = nullptr;
    int outFormat = 1;  //输出样本格式 AV_SAMPLE_FMT_S16
};
}
