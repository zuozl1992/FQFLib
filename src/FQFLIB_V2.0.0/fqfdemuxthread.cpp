#include "fqfdemuxthread.h"
#include "fqfaudiothread.h"
#include "fqfvideothread.h"
#include "fqftools.h"
#include "fqfdemux.h"
using namespace FQF;

FQFDemuxThread::FQFDemuxThread(FQFVideoDevice *vcall,FQFAudioDevice *acall){
    if (!demux) demux = new FQFDemux();
    if (!at) at = new FQFAudioThread();
    if (!vt) vt = new FQFVideoThread();
    startTh();
    vt->setCallBack(vcall);
    at->setCallBack(acall);
}

FQFDemuxThread::~FQFDemuxThread()
{
    isExit = true;
    wait();
    exitFileTh();
}

void FQFDemuxThread::setVideoDeviceCallBack(FQFVideoDevice *call)
{
    if(vt) vt->setCallBack(call);
}

void FQFDemuxThread::setAudioDeviceCallBack(FQFAudioDevice *call)
{
    if(at) at->setCallBack(call);
}

bool FQFDemuxThread::openFileTh(const char * path, int type)
{
    if (!path || path[0] == '\0')
		return false;

    setPause(true);
    closeFileTh();

	mux.lock();
    if (!demux)
    {
        mux.unlock();
        return false;
    }
    bool re = demux->openDemux(path, static_cast<FQFDemux::OpenType>(type));
	if (!re)
	{
        closeFileTh();
		mux.unlock();
		return false;
	}
    this->status = Play;
	bool re2 = true;
    if (!at)
    {
        closeFileTh();
        mux.unlock();
        return false;
    }
    if (!at->openDecodeTh(demux->copyAudioPara(),
                          demux->getFileSampleRate(),
                          demux->getFileChannels()))
	{
        if(type == FQFDemux::AudioFile || type == FQFDemux::AudioStream)
        {
            closeFileTh();
            re2 = false;
        }
    }
    else {
        at->setPause(false);
    }
    if (!vt)
    {
        closeFileTh();
        mux.unlock();
        return false;
    }
    if (!vt->openDecodeTh(demux->copyVideoPara(),
                          demux->getWidth(),
                          demux->getHeight()))
    {
        if(type == FQFDemux::VideoFile || type == FQFDemux::VideoStream)
        {
            closeFileTh();
            re2 = false;
        }
    }
    else {
        vt->setPause(false);
    }
    if(type >= 2)
        vt->setIsSyn(false);
    else
        vt->setIsSyn(true);
    totalMs = demux->getFileTimeMs();
    if(re2)
        status = Play;
    if(!re2)
        closeFileTh();
	mux.unlock();
	return re2;
}

void FQFDemuxThread::startTh()
{
	mux.lock();

    if(!this->isRunning()) QThread::start();
    if(at)
    {
        if(!at->isRunning())
            at->start();
    }
    if(vt)
    {
        if(!vt->isRunning())
            vt->start();
    }
	mux.unlock();
}

void FQFDemuxThread::closeFileTh()
{
    if (at) at->closeDecodeTh();
    if (vt) vt->closeDecodeTh();
    if (demux) demux->closeDemux();
}

void FQFDemuxThread::exitFileTh()
{
    isExit = true;
    wait();
    if (at) at->exitDecodeTh();
    if (vt) vt->exitDecodeTh();
    if (demux) demux->exitDemux();
    mux.lock();
    if(at){
    delete at;
    at = nullptr;
    }
    if(vt){
    delete vt;
    vt = nullptr;
    }
    if(demux){
    delete demux;
    demux = nullptr;
    }
    mux.unlock();
}

void FQFDemuxThread::clearFileTh()
{
	mux.lock();
    if (demux)demux->clearDemux();
    pts = 0;
    if (at)at->clearDecodeTh();
    if (vt)vt->clearDecodeTh();
	mux.unlock();
}

void FQFDemuxThread::setPause(bool isPause)
{
	mux.lock();
    if(demux->getMediaType() > 3)
    {
        mux.unlock();
        return;
    }
    if(status == Stop && isPause == true)
    {
        mux.unlock();
        return;
    }
    this->status = isPause ? Pause : Play;
    if (at)at->setPause(isPause);
    if (vt)vt->setPause(isPause);
	mux.unlock();
}

void FQFDemuxThread::seek(double pos)
{
    if(demux->getMediaType() > 3)
        return;

    mux.lock();
    bool bstatus = this->status == Play ? false : true;
    mux.unlock();

    setPause(true);
    clearFileTh();

    mux.lock();
    if(!demux)
    {
        mux.unlock();
        setPause(bstatus);
        return;
    }
    if(!demux->seek(pos))
    {
        mux.unlock();
        setPause(bstatus);
        return;
    }
    long long seekPts = static_cast<long long>(pos * demux->getFileTimeMs());
    this->pts = seekPts;
    if(at)
    {
        at->clearPcmBuffer();
        at->setPlayingMs(seekPts);
    }
    if(vt)
    {
        vt->setPlayingMs(seekPts);
        while (!isExit)
        {
            AVPacket *pkt = demux->readFile();
            if (!pkt)
                break;

            if (vt->RepaintPts(pkt, seekPts))
            {
                this->pts = seekPts;
                break;
            }
        }
    }
    mux.unlock();
    setPause(bstatus);
}

FQFDemuxThread::MusicType FQFDemuxThread::getMusicStatus()
{
    return status;
}

long long FQFDemuxThread::getNowTimeMs()
{
    return this->pts;
}

long long FQFDemuxThread::getFileTimeMs()
{
    return totalMs;
}

int FQFDemuxThread::getVideoWidth()
{
    return demux->getWidth();
}

int FQFDemuxThread::getVideoHeight()
{
    return demux->getHeight();
}

void FQFDemuxThread::run()
{
    while (!isExit)
    {
        mux.lock();
        if (status != Play)
        {
            mux.unlock();
            msleep(5);
            continue;
        }
        if (!demux)
        {
            mux.unlock();
            msleep(5);
            continue;
        }

        if (at) {
            pts = at->getPlayingMs();
            if(vt) vt->setSynPts(pts);
        }
        AVPacket *pkt = demux->readFile();
        if (!pkt)
        {
            if(at){
                if(demux->getMediaType() >= 2)
                {
                    if(at->getDecodeType() == FQFDecodeThread::End)
                        status = End;
                    }
                at->setFinishedReading();
            }
            if(vt){
                if(demux->getMediaType() < 2)
                {
                    if(vt->getDecodeType() == FQFDecodeThread::End)
                        status = End;
                    }
                vt->setFinishedReading();
            }
            mux.unlock();
            msleep(5);
            continue;
        }
        if (demux->getPktType(pkt) == FQFDemux::AudioPacket)
        {
            if(at) at->pushPacketToBufferList(pkt);
        }
        else if (demux->getPktType(pkt) == FQFDemux::VideoPacket)
        {
            if(vt) vt->pushPacketToBufferList(pkt);
        }
        else
        {
            FQFTools::FQFFreePacket(&pkt);
        }
        mux.unlock();
        msleep(1);
    }
}
