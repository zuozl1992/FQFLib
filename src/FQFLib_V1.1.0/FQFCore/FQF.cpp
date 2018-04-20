#include "FQF.h"
extern "C"
{
	#include <libavformat/avformat.h>
}

FQF::FQF()
{
	av_register_all();
	avformat_network_init();
}


FQF::~FQF()
{
}
