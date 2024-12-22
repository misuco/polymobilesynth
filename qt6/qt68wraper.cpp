/*
 
 Copyright (C) 2013 by Claudio Zopfi, Zurich, Suisse, z@x21.ch
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

#include <QtGlobal>
#ifndef Q_OS_IOS

#include "qt68wraper.h"

#include <QAudioDevice>
#include <QMediaDevices>
#include <QAudioSink>
#include <QDebug>
#include <qmath.h>
#include <qendian.h>


mobileSynthQT68::mobileSynthQT68()
    :   QIODevice()
   // ,   m_device(QMediaDevices::defaultAudioOutput())

{
    BufferSize        = 1024;
    //BufferSize        = 16384;

    /*
    qDebug() << "mobileSynthQT52::size " << size();
    for(auto &device:QMediaDevices::audioOutputs()) {
        qDebug() << "****" << device.id() << " " << device.description() ;
        if( device.isDefault()) {
            m_device=device;
            qDebug() << "------------------------------------------";
        }
    }
    */

    syctl = new synth::Controller();

    QAudioDevice info(QMediaDevices::defaultAudioOutput());
    m_format=info.preferredFormat();
    qDebug() << ">>>>" << info.id() << " " << info.description() ;
    qDebug() << "Preferred format is sr " << m_format.sampleRate() << " sn " << m_format.bytesPerSample() << " ch " << m_format.channelCount() << " sample format " << m_format.sampleFormat();

    DataSampleRateHz  = m_format.sampleRate();

    syctl->set_sample_rate(m_format.sampleRate());
    syctl->set_volume(0.8);
    syctl->set_osc1_level(0.8);
    syctl->set_arpeggio_enabled(true);
    syctl->set_arpeggio_octaves(1);
    syctl->set_arpeggio_samples(50000);
    syctl->set_arpeggio_step(synth::Arpeggio::UP_DOWN);
    syctl->set_filter_cutoff(1000);
    syctl->set_filter_resonance(0.1);


    channelBytes = m_format.bytesPerSample();
    channelCount = m_format.channelCount();
    sampleType = m_format.sampleFormat();// == QAudioFormat::UInt8 ? 0 : 1;
    sampleLittleEndian = QSysInfo::ByteOrder == 1 ? true : false;

    qDebug() << "sampleType " <<  sampleType << " channelCount " << channelCount << " channelBytes " << channelBytes << " sampleLittleEndian " << sampleLittleEndian;
    //syctl->setFormat(sampleType,channelCount,channelBytes,sampleLittleEndian);

    /*

    m_audioOutput=new QAudioSink(QMediaDevices::defaultAudioOutput(), m_format);
    //m_audioOutput->reset( = new QAudioOutput(m_device, this);
    //m_audioOutput->setBufferSize(BufferSize);

    this->start();
    //qDebug() << "buffer size is " << m_audioOutput->bufferSize();

    m_audioOutput->reset();
    m_audioOutput->resume();
    m_audioOutput->start(this);
    */

}

mobileSynthQT68::~mobileSynthQT68()
{
    //m_audioOutput->stop();
    this->stop();
    delete(syctl);
    delete(m_audioOutput);
}

void mobileSynthQT68::start()
{
    qDebug() << "mobileSynthQT68::start";
    open(QIODevice::ReadOnly);
}

void mobileSynthQT68::stop()
{
    qDebug() << "mobileSynthQT68::stop";
    close();
}

qint64 mobileSynthQT68::readData(char *data, qint64 len)
{
    qint64 len_ret=len;
    //qDebug() << "mobileSynthQT68::readData " << len ;

    //TODO: why this dirty hack? why does windows request odd lens
    /*
    if(len%2!=0) {
        qDebug() << "mobileSynthQT52::readData odd len " << len;
        len-=1;
    }
    */

    if(channelBytes>0) {
        unsigned char *ptr = reinterpret_cast<unsigned char *>(data);
        //int sampleBytes = channelCount * channelBytes;

        if (channelBytes == 1 && sampleType == 0 /* QAudioFormat::UnSignedInt */) {
            while (len>0) {
                qreal x=syctl->GetSample();

                const quint8 value = static_cast<quint8>((1.0 + x) / 2 * 255);
                *reinterpret_cast<quint8*>(ptr) = value;

                ptr += channelBytes;
                len -= channelBytes;
            }
        } else if (channelBytes == 1  && sampleType == 1 /* QAudioFormat::SignedInt */) {
            while (len>0) {
                qreal x=syctl->GetSample();

                const qint8 value = static_cast<qint8>(x * 127);
                *reinterpret_cast<quint8*>(ptr) = value;

                ptr += channelBytes;
                len -= channelBytes;
            }

        } else if (channelBytes == 2 && sampleType == 0 && sampleLittleEndian == true /* QAudioFormat::UnSignedInt */) {
            while (len>0) {
                qreal x=syctl->GetSample();

                quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
                qToLittleEndian<quint16>(value, ptr);

                ptr += channelBytes;
                len -= channelBytes;
            }
        } else if (channelBytes == 2 && sampleType == 0 && sampleLittleEndian == false /* QAudioFormat::UnSignedInt */) {
            while (len>0) {
                qreal x=syctl->GetSample();

                quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
                qToBigEndian<quint16>(value, ptr);

                ptr += channelBytes;
                len -= channelBytes;
            }
        } else if (channelBytes == 2 && sampleType == 1 && sampleLittleEndian == true  /* QAudioFormat::SignedInt */) {
            while (len>0) {
                qreal x=syctl->GetSample();

                qint16 value = static_cast<qint16>(x * 32767);
                qToLittleEndian<qint16>(value, ptr);

                ptr += channelBytes;
                len -= channelBytes;
            }
        } else if (channelBytes == 2 && sampleType == 1 && sampleLittleEndian == false  /* QAudioFormat::SignedInt */) {
            while (len>0) {
                qreal x=syctl->GetSample();

                qint16 value = static_cast<qint16>(x * 32767);
                qToBigEndian<qint16>(value, ptr);

                ptr += channelBytes;
                len -= channelBytes;
            }
        } else if (channelBytes == 4 && sampleType == 4 /* QAudioFormat::Float */) {
            while (len>0) {
                qreal x=syctl->GetSample();

                const quint8 value = static_cast<quint8>((1.0 + x) / 2 * 255);
                *reinterpret_cast<float*>(ptr) = x;

                ptr += channelBytes;
                len -= channelBytes;
            }
        }
    }

    //qDebug() << "mobileSynthQT68::readData done " << len ;
    return len_ret;
}

qint64 mobileSynthQT68::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 mobileSynthQT68::bytesAvailable() const
{
    //qDebug() << "mobileSynthQT68::bytesAvailable";
    return 16384;
}

qint64 mobileSynthQT68::size() const
{
    //qDebug() << "mobileSynthQT68::size";
    return 16384;
}

void mobileSynthQT68::noteOn(int vid, float f)
{
    qDebug() << "mobileSynthQT68::noteOn vid:" << vid << " f: " << f;
    //syctl->setADSR(0,10000,10000,1.0f,80000);

    //syctl->NoteOn(vid,f);
    syctl->NoteOn(vid);
}

void mobileSynthQT68::noteOff(int vid)
{
    qDebug() << "mobileSynthQT68::noteOff vid:" << vid;
    syctl->NoteOff(vid);
}

#endif
