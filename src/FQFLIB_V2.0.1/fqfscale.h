#pragma once
#include <mutex>
struct AVCodecParameters;
struct SwsContext;
struct AVFrame;
namespace FQF {
class FQFScale
{
public:
    FQFScale();
    virtual ~FQFScale();
    //打开转码器，输出参数与输入参数一致，除了采样格式，输出采样格式为s16
    //@param para 媒体格式
    //@param isClearPara 是否要清理传入的para，为true时清理
    //@return bool 打开结果
    virtual bool openScale(AVCodecParameters *para,bool isClearPara = false);
    //关闭转码器，释放空间
    virtual void closeScale();
    //退出转码器，释放空间
    virtual void exitScale();
    //进行重采样，不管成功与否都释放indata
    //@param inData 输入数据
    //@param outData 输出数据
    //@return int 完成的高
    virtual int scale(AVFrame *inData, AVFrame **outData);
    //进行重采样，不管成功与否都释放indata
    //@param inData 输入数据
    //@param outData 输出数据
    //@param width 转码后的宽
    //@param height 转码后的高
    //@return int 完成的高
    virtual int scale(AVFrame *inData, unsigned char *outData,int *width,int *height);

protected:
	std::mutex mux;
    SwsContext *vctx = nullptr;
    int outFormat = 28; //输出样本格式 AV_PIX_FMT_BGRA
    unsigned char *buf = nullptr;
};
}
