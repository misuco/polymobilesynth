// /^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\v/^\
// Copyright (C) 2024 c1audio.com / Claudio Zopfi <c1audio@x21.ch>
// SPDX-License-Identifier: GPL-3.0

#include <QtGlobal>
#ifndef Q_OS_IOS

#ifndef QT68WRAPER_H
#define QT68WRAPER_H

#include <QAudioFormat>
#include <QAudioOutput>
#include <QAudioDevice>
#include <QAudioSink>
#include <QByteArray>
#include <QIODevice>
#include <QObject>
#include <QScopedPointer>
#include <QTimer>
#include <QTimerEvent>

#include <polymobilesynth/mobilesynth/Classes/synth/controller.h>

class Qt68Wraper : public QIODevice
{
    Q_OBJECT

public:
    Qt68Wraper();
    ~Qt68Wraper();

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;
    qint64 size() const override;

    void set_arpeggio_enabled(bool v);
    void set_arpeggio_samples(int v);
    void set_arpeggio_octaves(int v);
    void set_arpeggio_step(int v);
    void set_filter_cutoff(int v);
    void set_filter_resonance(float v);
    void set_osc1_level(float v);
    void set_osc1_wave_type(synth::Oscillator::WaveType v);
    void set_osc1_octave(synth::Controller::OctaveShift v);
    void set_osc2_level(float v);
    void set_osc2_wave_type(synth::Oscillator::WaveType v);
    void set_osc2_octave(synth::Controller::OctaveShift v);
    void set_osc2_shift(int v);
    void set_modulation_source(synth::Controller::ModulationSource v);
    void set_modulation_destination(synth::Controller::ModulationDestination v);
    void set_modulation_amount(float v);
    void set_modulation_frequency(float v);

public slots:
    void noteOn(int vid, float f);
    void noteOff(int vid);

private:
    int DataSampleRateHz;
    int BufferSize;
    int channelBytes;
    int channelCount;
    int sampleType;
    bool sampleLittleEndian;

    QVector<synth::Controller *> syctl;
    QVector<int> VoiceMap;
    QAudioSink *m_audioOutput;
    QIODevice *m_output; // not owned
    QAudioFormat m_format;
    //QAudioDevice m_device;

    QTimer sampleTimer;

    int VoiceCount;
    qreal GetSampleSum();
};

#endif // QT68WRAPER_H
#endif // Q_OS_IOS
