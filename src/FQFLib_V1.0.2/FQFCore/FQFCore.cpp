#include "FQFCore.h"
extern "C"
{
#include <libavformat/avformat.h>
}
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"swscale.lib")

FQFCore::FQFCore()
{
    av_register_all();
    avformat_network_init();
}
