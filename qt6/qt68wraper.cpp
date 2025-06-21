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
    BufferSize = 1024;
    VoiceCount = 16;

    QAudioDevice info(QMediaDevices::defaultAudioOutput());
    m_format=info.preferredFormat();
    qDebug() << ">>>>" << info.id() << " " << info.description() ;
    qDebug() << "Preferred format is sr " << m_format.sampleRate() << " sn " << m_format.bytesPerSample() << " ch " << m_format.channelCount() << " sample format " << m_format.sampleFormat();

    DataSampleRateHz  = m_format.sampleRate();

    for(int i=0;i<VoiceCount;i++) {
        synth::Controller * sc = new synth::Controller();
        sc->set_sample_rate(m_format.sampleRate());
        sc->set_volume(0.8);
        sc->set_osc1_level(0.8);
        sc->set_arpeggio_enabled(false);
        sc->set_arpeggio_octaves(1);
        sc->set_arpeggio_samples(50000);
        sc->set_arpeggio_step(synth::Arpeggio::UP_DOWN);
        sc->set_filter_cutoff(1000);
        sc->set_filter_resonance(0.1);
        sc->volume_envelope()->set_attack(100);
        sc->volume_envelope()->set_decay(10000);
        sc->volume_envelope()->set_sustain(1.0f);
        sc->volume_envelope()->set_release(80000);
        sc->filter_envelope()->set_attack(80000);
        sc->filter_envelope()->set_decay(80000);
        sc->filter_envelope()->set_sustain(0.4f);
        sc->filter_envelope()->set_release(80000);
        syctl.append( sc );
        VoiceMap.append( 0 );
    }

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
    for(auto voice:syctl) {
        delete(voice);
    }
    syctl.clear();
    VoiceMap.clear();
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

qreal Qt68Wraper::GetSampleSum() {
    qreal x=0;
    for(auto voice:syctl) {
        x+=voice->GetSample();
    }
    x = fmaxf(-1.0f, x);
    x = fminf(1.0f, x);
    return x;
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
                qreal x=GetSampleSum();

                const quint8 value = static_cast<quint8>((1.0 + x) / 2 * 255);
                *reinterpret_cast<quint8*>(ptr) = value;

                ptr += channelBytes;
                len -= channelBytes;
            }
        } else if (channelBytes == 1  && sampleType == 1 /* QAudioFormat::SignedInt */) {
            while (len>0) {
                qreal x=GetSampleSum();

                const qint8 value = static_cast<qint8>(x * 127);
                *reinterpret_cast<quint8*>(ptr) = value;

                ptr += channelBytes;
                len -= channelBytes;
            }

        } else if (channelBytes == 2 && sampleType == 0 && sampleLittleEndian == true /* QAudioFormat::UnSignedInt */) {
            while (len>0) {
                qreal x=GetSampleSum();

                quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
                qToLittleEndian<quint16>(value, ptr);

                ptr += channelBytes;
                len -= channelBytes;
            }
        } else if (channelBytes == 2 && sampleType == 0 && sampleLittleEndian == false /* QAudioFormat::UnSignedInt */) {
            while (len>0) {
                qreal x=GetSampleSum();

                quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
                qToBigEndian<quint16>(value, ptr);

                ptr += channelBytes;
                len -= channelBytes;
            }
        } else if (channelBytes == 2 && sampleType == 1 && sampleLittleEndian == true  /* QAudioFormat::SignedInt */) {
            while (len>0) {
                qreal x=GetSampleSum();

                qint16 value = static_cast<qint16>(x * 32767);
                qToLittleEndian<qint16>(value, ptr);

                ptr += channelBytes;
                len -= channelBytes;
            }
        } else if (channelBytes == 2 && sampleType == 1 && sampleLittleEndian == false  /* QAudioFormat::SignedInt */) {
            while (len>0) {
                qreal x=GetSampleSum();

                qint16 value = static_cast<qint16>(x * 32767);
                qToBigEndian<qint16>(value, ptr);

                ptr += channelBytes;
                len -= channelBytes;
            }
        } else if (channelBytes == 4 && sampleType == 4 /* QAudioFormat::Float */) {
            while (len>0) {
                qreal x=GetSampleSum();

                //const quint8 value = static_cast<quint8>((1.0 + x) / 2 * 255);
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

void Qt68Wraper::set_arpeggio_enabled(bool v)
{
    for(auto voice:syctl) {
        voice->set_arpeggio_enabled(v);
    }
}

void Qt68Wraper::set_arpeggio_samples(int v)
{
    for(auto voice:syctl) {
        voice->set_arpeggio_samples(v);
    }
}

void Qt68Wraper::set_arpeggio_octaves(int v)
{
    for(auto voice:syctl) {
        voice->set_arpeggio_octaves(v);
    }
}

void Qt68Wraper::set_arpeggio_step(int v)
{
    synth::Arpeggio::Step step;

    switch(v) {
    case 0:
        step=synth::Arpeggio::Step::DOWN;
        break;
    case 1:
        step=synth::Arpeggio::Step::UP;
        break;
    case 2:
        step=synth::Arpeggio::Step::RANDOM;
        break;
    default:
        step=synth::Arpeggio::Step::UP_DOWN;
        break;
    }

    for(auto voice:syctl) {
        voice->set_arpeggio_step(step);
    }
}

void Qt68Wraper::set_filter_cutoff(int v)
{
    for(auto voice:syctl) {
        voice->set_filter_cutoff(v);
    }
}

void Qt68Wraper::set_filter_resonance(float v)
{
    for(auto voice:syctl) {
        voice->set_filter_resonance(v);
    }
}

void Qt68Wraper::set_osc1_level(float v)
{
    for(auto voice:syctl) {
        voice->set_osc1_level(v);
    }
}

void Qt68Wraper::set_osc1_wave_type(synth::Oscillator::WaveType v)
{
    for(auto voice:syctl) {
        voice->set_osc1_wave_type(v);
    }
}

void Qt68Wraper::set_osc1_octave(synth::Controller::OctaveShift v)
{
    for(auto voice:syctl) {
        voice->set_osc1_octave(v);
    }
}

void Qt68Wraper::set_osc2_level(float v)
{
    for(auto voice:syctl) {
        voice->set_osc2_level(v);
    }
}

void Qt68Wraper::set_osc2_wave_type(synth::Oscillator::WaveType v)
{
    for(auto voice:syctl) {
        voice->set_osc2_wave_type(v);
    }
}

void Qt68Wraper::set_osc2_octave(synth::Controller::OctaveShift v)
{
    for(auto voice:syctl) {
        voice->set_osc2_octave(v);
    }
}

void Qt68Wraper::set_osc2_shift(int v)
{
    for(auto voice:syctl) {
        voice->set_osc2_shift(v);
    }
}

void Qt68Wraper::set_modulation_source(synth::Controller::ModulationSource v)
{
    for(auto voice:syctl) {
        voice->set_modulation_source(v);
    }
}

void Qt68Wraper::set_modulation_destination(synth::Controller::ModulationDestination v)
{
    for(auto voice:syctl) {
        voice->set_modulation_destination(v);
    }
}

void Qt68Wraper::set_modulation_amount(float v)
{
    for(auto voice:syctl) {
        voice->set_modulation_amount(v);
    }
}

void Qt68Wraper::set_modulation_frequency(float v)
{
    for(auto voice:syctl) {
        voice->set_modulation_frequency(v);
    }
}

void Qt68Wraper::set_volume_attack(long v)
{
    for(auto voice:syctl) {
        voice->volume_envelope()->set_attack(v);
    }
}

void Qt68Wraper::set_volume_decay(long v)
{
    for(auto voice:syctl) {
        voice->volume_envelope()->set_decay(v);
    }
}

void Qt68Wraper::set_volume_sustain(float v)
{
    for(auto voice:syctl) {
        voice->volume_envelope()->set_sustain(v);
    }
}

void Qt68Wraper::set_volume_release(long v)
{
    for(auto voice:syctl) {
        voice->volume_envelope()->set_release(v);
    }
}

void Qt68Wraper::set_filter_attack(long v)
{
    for(auto voice:syctl) {
        voice->filter_envelope()->set_attack(v);
    }
}

void Qt68Wraper::set_filter_decay(long v)
{
    for(auto voice:syctl) {
        voice->filter_envelope()->set_decay(v);
    }
}

void Qt68Wraper::set_filter_sustain(float v)
{
    for(auto voice:syctl) {
        voice->filter_envelope()->set_sustain(v);
    }
}

void Qt68Wraper::set_filter_release(long v)
{
    for(auto voice:syctl) {
        voice->filter_envelope()->set_release(v);
    }
}

void Qt68Wraper::noteOn(int vid, float f)
{
    qDebug() << "mobileSynthQT68::noteOn vid:" << vid << " f: " << f;
    //syctl->setADSR(0,10000,10000,1.0f,80000);
    for(int i=0;i<VoiceMap.count();i++) {
        if(syctl.at(i)->released()) {
            syctl.at(i)->NoteOnFrequency(f);
            VoiceMap[i]=vid;
            qDebug() << "mobileSynthQT68::noteOn mapid:" << i;
            break;
        }
    }
}

void Qt68Wraper::noteOff(int vid)
{
    qDebug() << "mobileSynthQT68::noteOff vid:" << vid;
    for(int i=0;i<VoiceMap.count();i++) {
        if(VoiceMap.at(i)==vid) {
            syctl.at(i)->NoteOff(vid);
            qDebug() << "mobileSynthQT68::noteOff mapid:" << i;
            break;
        }
    }
}

#endif
