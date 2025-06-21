// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MOBILESYNTH_H
#define MOBILESYNTH_H

#include <QAudioSink>
#include <QByteArray>
#include <QIODevice>
#include <QMediaDevices>
#include <QObject>
#include <QScopedPointer>
#include <QTimer>

#include <polymobilesynth/qt6/qt68wraper.h>

#include <math.h>

class MobileSynth : public QObject
{
    Q_OBJECT

public:
    MobileSynth();
    ~MobileSynth();

    Q_INVOKABLE void noteOn(int vid, float f);
    Q_INVOKABLE void noteOff(int vid);
    Q_INVOKABLE void set_arpeggio_enabled(bool value);
    Q_INVOKABLE void set_arpeggio_samples(int value);
    Q_INVOKABLE void set_arpeggio_octaves(int value);
    Q_INVOKABLE void set_arpeggio_step(int value);
    Q_INVOKABLE void set_filter_cutoff(int value);
    Q_INVOKABLE void set_filter_resonance(float value);
    Q_INVOKABLE void set_osc1_level(float value);
    Q_INVOKABLE void set_osc1_wave_type(int value);
    Q_INVOKABLE void set_osc1_octave(int octave);
    Q_INVOKABLE void set_osc2_level(float value);
    Q_INVOKABLE void set_osc2_wave_type(int value);
    Q_INVOKABLE void set_osc2_octave(int octave);
    Q_INVOKABLE void set_osc2_shift(int value);
    Q_INVOKABLE void set_modulation_source(int value);
    Q_INVOKABLE void set_modulation_destination(int value);
    Q_INVOKABLE void set_modulation_amount(float value);
    Q_INVOKABLE void set_modulation_frequency(float value);
    Q_INVOKABLE void set_volume_attack(long value);
    Q_INVOKABLE void set_volume_decay(long value);
    Q_INVOKABLE void set_volume_sustain(float value);
    Q_INVOKABLE void set_volume_release(long value);
    Q_INVOKABLE void set_filter_attack(long value);
    Q_INVOKABLE void set_filter_decay(long value);
    Q_INVOKABLE void set_filter_sustain(float value);
    Q_INVOKABLE void set_filter_release(long value);

private:
    void initializeAudio(const QAudioDevice &deviceInfo);

private:
    QMediaDevices *m_devices = nullptr;
    QTimer *m_pushTimer = nullptr;

    //QScopedPointer<Generator> m_generator;
    QScopedPointer<Qt68Wraper> m_generator;
    QScopedPointer<QAudioSink> m_audioOutput;

    bool m_pullMode = true;

    synth::Oscillator::WaveType int2wavetype(int value);
    synth::Controller::OctaveShift int2octaveshift(int value);

private slots:
    void toggleMode();
    void toggleSuspendResume();
    //void deviceChanged(int index);
    void volumeChanged(int);
    //void updateAudioDevices();
};

#endif // MOBILESYNTH_H
