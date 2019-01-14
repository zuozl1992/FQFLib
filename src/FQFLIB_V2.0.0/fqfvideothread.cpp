#include "fqfvideothread.h"
#include "fqfvideodevice.h"
#include "fqfdecode.h"
#include "fqfscale.h"
#include "fqftools.h"
using namespace FQF;
#define RGBSIZE (4096 * 4096 * 4)

FQFVideoThread::FQFVideoThread(){
    sca = new FQFScale;
    rgb = new unsigned char[RGBSIZE];
}

FQFVideoThread::~FQFVideoThread(){}

void FQFVideoThread::setCallBack(FQFVideoDevice *call)
{
    vmux.lock();
    this->vd = call;
    vmux.unlock();
}


bool FQFVideoThread::openDecodeTh(AVCodecParameters * para, int width, int height)
{
    if (!para)
    {
        openSu = false;
        return false;
    }
    closeDecodeTh();
	vmux.lock();
    synPts = 0;
	bool re = true;
    if (!sca)
    {
        openSu = false;
        vmux.unlock();
        return false;
    }
    if (!sca->openScale(para,false)) re = false;
    if (!vd)
    {
        openSu = false;
        vmux.unlock();
        return false;
    }
    if (!vd->openDevice(width, height)) re = false;
    if (!decode)
    {
        openSu = false;
        vmux.unlock();
        return false;
    }
    if (!decode->openDecode(para)) re = false;
    if(re){
        status = FQFDecodeThread::Play;
        isFinishedReading = false;
    }
    openSu = re;
    vmux.unlock();
    return re;
}

void FQFVideoThread::closeDecodeTh()
{
    FQFDecodeThread::closeDecodeTh();
    vmux.lock();
    synPts = 0;
    pts = 0;
    if(sca)
    {
        sca->closeScale();
    }
    if(vd)
    {
        vd->closeDevice();
    }
    vmux.unlock();
}

void FQFVideoThread::exitDecodeTh()
{
    FQFDecodeThread::exitDecodeTh();
    vmux.lock();
    synPts = 0;
    pts = 0;
    if(rgb)
    {
        delete [] rgb;
        rgb = nullptr;
    }
    if(sca)
    {
        sca->exitScale();
        delete sca;
        sca = nullptr;
    }
    if(vd)
    {
        vd->exitDevice();
        vd = nullptr;
    }
    vmux.unlock();
}

void FQFVideoThread::clearDecodeTh()
{
    FQFDecodeThread::clearDecodeTh();
    vmux.lock();
    synPts = 0;
    pts = 0;
    vmux.unlock();
}

void FQFVideoThread::setPause(bool isPause)
{
    vmux.lock();
    if(status == FQFDecodeThread::Stop && isPause == true)
    {
        return;
    }
    this->status = isPause ? FQFDecodeThread::Pause : FQFDecodeThread::Play;
    vmux.unlock();
}

void FQFVideoThread::setSynPts(long long pts)
{
    vmux.lock();
    synPts = pts;
    vmux.unlock();
}

void FQFVideoThread::setPlayingMs(long long pts)
{
    vmux.lock();
    this->pts = pts;
    isFinishedReading = false;
    vmux.unlock();
}

void FQFVideoThread::setIsSyn(bool ok)
{
    vmux.lock();
    isSyn = ok;
    vmux.unlock();
}

bool FQFVideoThread::RepaintPts(AVPacket * pkt, long long seekpts)
{
    vmux.lock();
    bool re = decode->sendPacketToDecode(pkt);
    if (!re)
    {
        vmux.unlock();
        return true;	//表示结束解码
    }
    AVFrame *frame = decode->recvFrameFromDecode();
    if (!frame)
    {
        vmux.unlock();
        return false;
    }
    pts = decode->getDecodingMs();
    if (decode->getDecodingMs() >= seekpts)
    {
        if(vd)
        {
            int width,height;
            if(sca->scale(frame,rgb,&width,&height)>0)
            {
                vd->writeToDeviceBuffer(rgb,width,height);
            }
        }
        vmux.unlock();
        return true;
    }
    FQFTools::FQFFreeFrame(&frame);
    vmux.unlock();
    return false;
}

void FQFVideoThread::run()
{
	while (!isExit)
	{
		vmux.lock();
        if(!openSu)
        {
            vmux.unlock();
            msleep(10);
            continue;
        }

        if (status != FQFDecodeThread::Play)
		{
			vmux.unlock();
			msleep(5);
			continue;
		}
        if(isSyn)
        {
            if (synPts > 0 && synPts < pts)
            {

                vmux.unlock();
                msleep(2);
                continue;
            }
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
                    vmux.unlock();
                    msleep(1);
                    continue;
                }
            }
            else {
                vmux.unlock();
                msleep(1);
                continue;
            }
        }
        vmux.unlock();
		//一次send多次recv
		while (!isExit)
		{
            vmux.lock();
            AVFrame *frame = decode->recvFrameFromDecode();
            if (!frame)
            {
                vmux.unlock();
                break;
            }
            pts = decode->getDecodingMs();
			//显示视频
            if (vd)
			{
                int width,height;
                if(sca->scale(frame,rgb,&width,&height)>0)
                {
                    vd->writeToDeviceBuffer(rgb,width,height);
                }
			}
            vmux.unlock();
		}
	}
}


