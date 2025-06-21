// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "mobilesynth.h"

#include <QAudioDevice>
#include <QAudioSink>
#include <QDebug>
#include <QtEndian>
#include <QtMath>

void MobileSynth::noteOn(int vid, float f)
{
    m_generator->noteOn(vid,f);
}

void MobileSynth::noteOff(int vid)
{
    m_generator->noteOff(vid);
}

void MobileSynth::set_arpeggio_enabled(bool value)
{
    m_generator->set_arpeggio_enabled(value);
}

void MobileSynth::set_arpeggio_samples(int value)
{
    m_generator->set_arpeggio_samples(value);
}

void MobileSynth::set_arpeggio_octaves(int value)
{
    m_generator->set_arpeggio_octaves(value);
}

void MobileSynth::set_arpeggio_step(int value)
{
    m_generator->set_arpeggio_step(value);
}

void MobileSynth::set_filter_cutoff(int value)
{
    m_generator->set_filter_cutoff(value);
}

void MobileSynth::set_filter_resonance(float value)
{
    m_generator->set_filter_resonance(value);
}

void MobileSynth::set_osc1_level(float value)
{
    m_generator->set_osc1_level(value);
}

synth::Oscillator::WaveType MobileSynth::int2wavetype(int value) {
    synth::Oscillator::WaveType w;
    switch (value) {
    case 1:
        w=synth::Oscillator::WaveType::TRIANGLE;
        break;
    case 2:
        w=synth::Oscillator::WaveType::SAWTOOTH;
        break;
    case 3:
        w=synth::Oscillator::WaveType::REVERSE_SAWTOOTH;
        break;
    case 4:
        w=synth::Oscillator::WaveType::SQUARE;
        break;
    default:
        w=synth::Oscillator::WaveType::SINE;
        break;
    }
    return w;
}

synth::Controller::OctaveShift MobileSynth::int2octaveshift(int value) {
    synth::Controller::OctaveShift o;
    switch (value) {
    case 1:
        o=synth::Controller::OctaveShift::OCTAVE_2;
        break;
    case 2:
        o=synth::Controller::OctaveShift::OCTAVE_4;
        break;
    case 3:
        o=synth::Controller::OctaveShift::OCTAVE_8;
        break;
    case 4:
        o=synth::Controller::OctaveShift::OCTAVE_16;
        break;
    default:
        o=synth::Controller::OctaveShift::OCTAVE_1;
        break;
    }
    return o;
}

void MobileSynth::set_osc1_wave_type(int value)
{
    m_generator->set_osc1_wave_type(int2wavetype(value));
}

void MobileSynth::set_osc1_octave(int octave)
{
    m_generator->set_osc1_octave(int2octaveshift(octave));
}

void MobileSynth::set_osc2_level(float value)
{
    m_generator->set_osc2_level(value);
}

void MobileSynth::set_osc2_wave_type(int value)
{
    m_generator->set_osc2_wave_type(int2wavetype(value));
}

void MobileSynth::set_osc2_octave(int octave)
{
    m_generator->set_osc2_octave(int2octaveshift(octave));
}

void MobileSynth::set_osc2_shift(int value)
{
    m_generator->set_osc2_shift(value);
}

void MobileSynth::set_modulation_source(int value)
{
    m_generator->set_modulation_source(static_cast<synth::Controller::ModulationSource>(value));
}

void MobileSynth::set_modulation_destination(int value)
{
    m_generator->set_modulation_destination(static_cast<synth::Controller::ModulationDestination>(value));
}

void MobileSynth::set_modulation_amount(float value)
{
    m_generator->set_modulation_amount(value);
}

void MobileSynth::set_modulation_frequency(float value)
{
    m_generator->set_modulation_frequency(value);
}

void MobileSynth::set_volume_attack(long value)
{
    m_generator->set_volume_attack(value);
}

void MobileSynth::set_volume_decay(long value)
{
    m_generator->set_volume_decay(value);
}

void MobileSynth::set_volume_sustain(float value)
{
    m_generator->set_volume_sustain(value);
}

void MobileSynth::set_volume_release(long value)
{
    m_generator->set_volume_release(value);
}

void MobileSynth::set_filter_attack(long value)
{
    m_generator->set_filter_attack(value);
}

void MobileSynth::set_filter_decay(long value)
{
    m_generator->set_filter_decay(value);
}

void MobileSynth::set_filter_sustain(float value)
{
    m_generator->set_filter_sustain(value);
}

void MobileSynth::set_filter_release(long value)
{
    m_generator->set_filter_release(value);
}

MobileSynth::MobileSynth() : m_devices(new QMediaDevices(this)), m_pushTimer(new QTimer(this))
{
    initializeAudio(m_devices->defaultAudioOutput());
}

MobileSynth::~MobileSynth()
{
    m_pushTimer->stop();
}

void MobileSynth::initializeAudio(const QAudioDevice &deviceInfo)
{
    QAudioFormat format = deviceInfo.preferredFormat();

    const int durationSeconds = 1;
    const int toneSampleRateHz = 600;
    //m_generator.reset(new Generator(format, durationSeconds * 1000000, toneSampleRateHz));
    m_generator.reset(new Qt68Wraper());
    m_audioOutput.reset(new QAudioSink(deviceInfo, format));
    m_generator->start();

    /*
    qreal initialVolume = QAudio::convertVolume(m_audioOutput->volume(), QAudio::LinearVolumeScale,
                                                QAudio::LogarithmicVolumeScale);
    m_volumeSlider->setValue(qRound(initialVolume * 100));
    */
    toggleMode();
}

/*
void MobileSynth::deviceChanged(int index)
{
    m_generator->stop();
    m_audioOutput->stop();
    m_audioOutput->disconnect(this);
    initializeAudio(m_deviceBox->itemData(index).value<QAudioDevice>());
}
*/

void MobileSynth::volumeChanged(int value)
{
    qreal linearVolume = QAudio::convertVolume(value / qreal(100), QAudio::LogarithmicVolumeScale,
                                               QAudio::LinearVolumeScale);

    m_audioOutput->setVolume(linearVolume);
}

/*
void MobileSynth::updateAudioDevices()
{
    m_deviceBox->clear();
    const QList<QAudioDevice> devices = m_devices->audioOutputs();
    for (const QAudioDevice &deviceInfo : devices)
        m_deviceBox->addItem(deviceInfo.description(), QVariant::fromValue(deviceInfo));
}
*/

void MobileSynth::toggleMode()
{
    m_pushTimer->stop();
    // Reset audiosink
    m_audioOutput->reset();
    toggleSuspendResume();

    if (m_pullMode) {
        // switch to pull mode (QAudioSink pulls from Generator as needed)
        //m_modeButton->setText(tr("Enable push mode"));
        m_audioOutput->start(m_generator.data());
    } else {
        // switch to push mode (periodically push to QAudioSink using a timer)
        //m_modeButton->setText(tr("Enable pull mode"));
        auto io = m_audioOutput->start();
        m_pushTimer->disconnect();

        connect(m_pushTimer, &QTimer::timeout, [this, io]() {
            if (m_audioOutput->state() == QAudio::StoppedState)
                return;

            int len = m_audioOutput->bytesFree();
            QByteArray buffer(len, 0);
            len = m_generator->read(buffer.data(), len);
            if (len)
                io->write(buffer.data(), len);
        });

        m_pushTimer->start(10);
    }

    m_pullMode = !m_pullMode;
}

void MobileSynth::toggleSuspendResume()
{
    if (m_audioOutput->state() == QAudio::SuspendedState
        || m_audioOutput->state() == QAudio::StoppedState) {
        m_audioOutput->resume();
        //m_suspendResumeButton->setText(tr("Suspend playback"));
    } else if (m_audioOutput->state() == QAudio::ActiveState) {
        m_audioOutput->suspend();
        //m_suspendResumeButton->setText(tr("Resume playback"));
    } else if (m_audioOutput->state() == QAudio::IdleState) {
        // no-op
    }
}

//#include "moc_audiooutput.cpp"
