// /^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\
// Copyright (C) 2024 c1audio.com / Claudio Zopfi <c1audio@x21.ch>
// SPDX-License-Identifier: GPL-3.0

#include <QtGlobal>
#ifndef Q_OS_IOS

#include "qt68wraper.h"

#include <QAudioDevice>
#include <QMediaDevices>
#include <QAudioSink>
#include <QDebug>
#include <qmath.h>
#include <qendian.h>


Qt68Wraper::Qt68Wraper()
    :   QIODevice()
{
    BufferSize        = 1024;

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
}

Qt68Wraper::~Qt68Wraper()
{
    //m_audioOutput->stop();
    this->stop();
    delete(syctl);
    delete(m_audioOutput);
}

void Qt68Wraper::start()
{
    qDebug() << "mobileSynthQT68::start";
    open(QIODevice::ReadOnly);
}

void Qt68Wraper::stop()
{
    qDebug() << "mobileSynthQT68::stop";
    close();
}

qint64 Qt68Wraper::readData(char *data, qint64 len)
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

qint64 Qt68Wraper::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 Qt68Wraper::bytesAvailable() const
{
    //qDebug() << "mobileSynthQT68::bytesAvailable";
    return 16384;
}

qint64 Qt68Wraper::size() const
{
    //qDebug() << "mobileSynthQT68::size";
    return 16384;
}

void Qt68Wraper::noteOn(int vid, float f)
{
    qDebug() << "mobileSynthQT68::noteOn vid:" << vid << " f: " << f;
    //syctl->setADSR(0,10000,10000,1.0f,80000);

    //syctl->NoteOn(vid,f);
    syctl->NoteOn(vid);
}

void Qt68Wraper::noteOff(int vid)
{
    qDebug() << "mobileSynthQT68::noteOff vid:" << vid;
    syctl->NoteOff(vid);
}

#endif
