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

/*
class Generator : public QIODevice
{
    Q_OBJECT

public:
    Generator(const QAudioFormat &format, qint64 durationUs, int sampleRate);

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;
    qint64 size() const override { return m_buffer.size(); }

private:
    void generateData(const QAudioFormat &format, qint64 durationUs, int sampleRate);

private:
    qint64 m_pos = 0;
    QByteArray m_buffer;
};

*/

class AudioOutput : public QObject
{
    Q_OBJECT

public:
    AudioOutput();
    ~AudioOutput();

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

private:
    void initializeAudio(const QAudioDevice &deviceInfo);

private:
    QMediaDevices *m_devices = nullptr;
    QTimer *m_pushTimer = nullptr;

    //QScopedPointer<Generator> m_generator;
    QScopedPointer<mobileSynthQT68> m_generator;
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
