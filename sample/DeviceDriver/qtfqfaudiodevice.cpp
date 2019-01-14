#include "qtfqfaudiodevice.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <QDebug>

QtFQFAudioDevice::~QtFQFAudioDevice()
{
    closeDevice();
}
long long QtFQFAudioDevice::getBufferQueueLength()
{
    mux.lock();
    if (!output)
    {
        mux.unlock();
        return 0;
    }
    long long pts = 0;
    //未播放的字节数
    double size = output->bufferSize() - output->bytesFree();
    //一秒可以播放的字节大小
    double secSize = sampleRate * (sampleSize / 8) * channels;
    if (secSize <= 0)
    {
        mux.unlock();
        return 0;
    }
    pts = static_cast<long long>((size / secSize)) * 1000;
    mux.unlock();
    return pts;
}
void QtFQFAudioDevice::closeDevice()
{
    clearDevice();
    mux.lock();
    if (io)
    {
        io->close();
        io = nullptr;
    }
    if (output)
    {
        output->stop();
        delete output;
        output = nullptr;
    }
    mux.unlock();
}

void QtFQFAudioDevice::exitDevice()
{
    mux.lock();
    if (io)
    {
        io->close();
        io = nullptr;
    }
    if (output)
    {
        output->stop();
        delete output;
        output = nullptr;
    }
    mux.unlock();
}
void QtFQFAudioDevice::setPause(bool isPause)
{
    mux.lock();
    if (!output)
    {
        mux.unlock();
        return;
    }
    if (isPause)
        output->suspend();
    else
        output->resume();
    mux.unlock();
}
bool QtFQFAudioDevice::openDevice(int sampleRate, int channels)
{
    closeDevice();
    this->sampleRate = sampleRate;
    this->channels = channels;
    QAudioFormat fmt;
    fmt.setSampleRate(sampleRate);
    fmt.setSampleSize(sampleSize);
    fmt.setChannelCount(channels);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::UnSignedInt);
    mux.lock();
    output = new QAudioOutput(fmt);
    io = output->start();
    mux.unlock();
    if (io) return true;
    return false;
}
void QtFQFAudioDevice::clearDevice()
{
    mux.lock();
    if (io) io->reset();
    mux.unlock();
}
bool QtFQFAudioDevice::writeToDeviceBuffer(const unsigned char *data, int datasize)
{
    if (!data || datasize <= 0) return false;
    mux.lock();
    if (!output || !io)
    {
        mux.unlock();
        return false;
    }
    qint64 size = io->write(reinterpret_cast<const char *>(data), datasize);
    mux.unlock();
    if (datasize != size) return false;
    return true;
}

int QtFQFAudioDevice::getBufferLength()
{
    mux.lock();
    if (!output)
    {
        mux.unlock();
        return 0;
    }
    int free = output->bytesFree();
    mux.unlock();
    return free;
}
