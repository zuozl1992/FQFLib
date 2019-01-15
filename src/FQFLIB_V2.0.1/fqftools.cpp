#include "fqftools.h"
using namespace FQF;
extern "C"
{
#include <libavcodec/avcodec.h>
}

void FQFTools::FQFFreePacket(AVPacket **pkt)
{
    if (!pkt || !(*pkt)) return;
    av_packet_free(pkt);
}

void FQFTools::FQFFreeFrame(AVFrame **frame)
{
    if (!frame || !(*frame)) return;
    av_frame_free(frame);
}
