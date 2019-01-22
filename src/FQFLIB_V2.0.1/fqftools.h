#pragma once
struct AVPacket;
struct AVFrame;
namespace FQF {

class FQFTools
{
public:
	//释放pkt
    static void FQFFreePacket(AVPacket **pkt);
	//释放frame
    static void FQFFreeFrame(AVFrame **frame);
};
}
