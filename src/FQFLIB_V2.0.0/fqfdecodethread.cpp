#include "fqfdecodethread.h"
#include "fqfdecode.h"
#include "fqftools.h"
using namespace FQF;
FQFDecodeThread::FQFDecodeThread()
{
    if (!decode) decode = new FQFDecode();
}


FQFDecodeThread::~FQFDecodeThread()
{
	isExit = true;
	wait();
}

void FQFDecodeThread::pushPacketToBufferList(AVPacket * pkt)
{
	if (!pkt)
		return;
	while (!isExit)
	{
        mux.lock();
		if (packs.size() < maxList)
		{
			packs.push_back(pkt);
			mux.unlock();
			break;
		}
		mux.unlock();
        msleep(1);
	}
}

AVPacket * FQFDecodeThread::popPacketFromBufferList()
{
	mux.lock();

	if (packs.empty())
	{
		mux.unlock();
        return nullptr;
	}
	AVPacket *pkt = packs.front();
	packs.pop_front();
	mux.unlock();
	return pkt;
}

void FQFDecodeThread::clearDecodeTh()
{
	mux.lock();
    decode->clearDecode();
	while (!packs.empty())
	{
		AVPacket *pkt = packs.front();
        FQFTools::FQFFreePacket(&pkt);
		packs.pop_front();
	}
    packs.clear();
	mux.unlock();
}

void FQFDecodeThread::closeDecodeTh()
{
    clearDecodeTh();
    mux.lock();
    decode->closeDecode();
    openSu = false;
    status = FQFDecodeThread::End;
    mux.unlock();
}

void FQFDecodeThread::exitDecodeTh()
{
    closeDecodeTh();
    isExit = true;
    wait();
    mux.lock();
    delete decode;
    decode = nullptr;
    mux.unlock();
}

void FQFDecodeThread::setDecodeType(FQFDecodeThread::DecodeType s)
{
    mux.lock();
    this->status = s;
    mux.unlock();
}

FQFDecodeThread::DecodeType FQFDecodeThread::getDecodeType()
{
    return this->status;
}

void FQFDecodeThread::setFinishedReading()
{
    mux.lock();
    isFinishedReading = true;
    mux.unlock();
}
