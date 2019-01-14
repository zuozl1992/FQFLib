#include "fqfaudiothread.h"
#include "fqfdecode.h"
#include "fqfaudiodevice.h"
#include "fqfresample.h"
using namespace FQF;
#define PCMSIZE (1024 * 1024)

FQFAudioThread::FQFAudioThread()
{
    res = new FQFResample();
    pcm = new unsigned char[PCMSIZE];
}

FQFAudioThread::~FQFAudioThread()
{
    isExit = true;
    wait();
    delete [] pcm;
}

void FQFAudioThread::setCallBack(FQFAudioDevice *call)
{
    amux.lock();
    ad = call;
    amux.unlock();
}

bool FQFAudioThread::openDecodeTh(AVCodecParameters * para, int sampleRate, int channels)
{
    if (!para)
    {
        openSu = false;
        return false;
    }
    closeDecodeTh();
	amux.lock();
	pts = 0;
	bool re = true;
    if (!res)
    {
        openSu = false;
        amux.unlock();
        return false;
    }
    if (!res->openResample(para, false)) re = false;
    if (!ad)
    {
        openSu = false;
        amux.unlock();
        return false;
    }
    if (!ad->openDevice(sampleRate, channels)) re = false;
    if (!decode)
    {
        openSu = false;
        amux.unlock();
        return false;
    }
    if (!decode->openDecode(para)) re = false;
    if(re){
        status = FQFDecodeThread::Play;
        isFinishedReading = false;
    }
    openSu = re;
	amux.unlock();
	return re;
}

void FQFAudioThread::closeDecodeTh()
{
    FQFDecodeThread::closeDecodeTh();
    clearDecodeTh();
    amux.lock();
    pts = 0;
	if (res)
	{
        res->closeResample();
	}
    if (ad)
    {
        ad->closeDevice();
	}
    amux.unlock();
}

void FQFAudioThread::exitDecodeTh()
{
    FQFDecodeThread::exitDecodeTh();
    amux.lock();
    pts = 0;
    if (res)
    {
        res->exitResample();
        delete res;
        res = nullptr;
    }
    if (ad)
    {   ad->exitDevice();
        ad = nullptr;
    }
    amux.unlock();
}

void FQFAudioThread::clearDecodeTh()
{
    FQFDecodeThread::clearDecodeTh();
	mux.lock();
    if (ad)ad->clearDevice();
    pts = 0;
    memset(pcm,0,PCMSIZE);
	mux.unlock();
}

void FQFAudioThread::setPause(bool isPause)
{
    amux.lock();
    if(status == FQFDecodeThread::Stop && isPause == true)
    {
        amux.unlock();
        return;
    }
    this->status = isPause ? FQFDecodeThread::Pause : FQFDecodeThread::Play;

    if(ad)
        ad->setPause(isPause);
    amux.unlock();
}

long long FQFAudioThread::getPlayingMs()
{
    return pts;
}

void FQFAudioThread::setPlayingMs(long long pts)
{
    amux.lock();
    this->pts = pts;
    isFinishedReading = false;
    amux.unlock();
}

void FQFAudioThread::clearPcmBuffer()
{
    amux.lock();
    memset(pcm,0,PCMSIZE);
    amux.unlock();
}

void FQFAudioThread::run()
{

	while (!isExit)
	{
		amux.lock();

        if(!openSu)
        {
            amux.unlock();
            msleep(10);
            continue;
        }

        if (status != FQFDecodeThread::Play)
		{
			amux.unlock();
			msleep(5);
			continue;
		}

        AVPacket *pkt = popPacketFromBufferList();
        bool re = decode->sendPacketToDecode(pkt);
		if (!re)
		{
            if(isFinishedReading)
            {
                re = decode->sendNullptrToDecode();
                if(!re)
                {
                    status = FQFDecodeThread::End;
                    amux.unlock();
                    msleep(1);
                    continue;
                }
            }
		}
        amux.unlock();
		//一次send多次recv
		while(!isExit)
		{
            amux.lock();
            AVFrame *frame = decode->recvFrameFromDecode();
            if (!frame)
            {
                amux.unlock();
                break;
            }
			//减去缓冲中未播放的时间 ms
            pts = decode->getDecodingMs() - ((ad != nullptr) ? ad->getBufferQueueLength() : 0);
            int size = res->resample(frame, pcm);
            if(!ad)
            {
                amux.unlock();
                msleep(1);
                continue;
            }
            amux.unlock();

			//播放
			while (!isExit)
			{
                amux.lock();
				if (size <= 0)
					break;
                if (ad->getBufferLength() < size || status != FQFDecodeThread::Play)
				{
                    amux.unlock();
					msleep(1);
					continue;
				}
                ad->writeToDeviceBuffer(pcm, size);
				break;
			}
            amux.unlock();
		}

	}
}
