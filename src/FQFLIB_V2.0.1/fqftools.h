#pragma once
struct AVPacket;
struct AVFrame;
namespace FQF {

class FQFTools
{
public:
    static void FQFFreePacket(AVPacket **pkt);
    static void FQFFreeFrame(AVFrame **frame);
};
}
