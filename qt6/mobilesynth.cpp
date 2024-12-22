// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "mobilesynth.h"

#include <QAudioDevice>
#include <QAudioSink>
#include <QDebug>
#include <QtEndian>
#include <QtMath>

/*
Generator::Generator(const QAudioFormat &format, qint64 durationUs, int sampleRate)
{
    if (format.isValid())
        generateData(format, durationUs, sampleRate);
}

void Generator::start()
{
    open(QIODevice::ReadOnly);
}

void Generator::stop()
{
    m_pos = 0;
    close();
}

void Generator::generateData(const QAudioFormat &format, qint64 durationUs, int sampleRate)
{
    const int channelBytes = format.bytesPerSample();
    const int sampleBytes = format.channelCount() * channelBytes;
    qint64 length = format.bytesForDuration(durationUs);
    Q_ASSERT(length % sampleBytes == 0);
    Q_UNUSED(sampleBytes); // suppress warning in release builds

    m_buffer.resize(length);
    unsigned char *ptr = reinterpret_cast<unsigned char *>(m_buffer.data());
    int sampleIndex = 0;

    while (length) {
        // Produces value (-1..1)
        const qreal x = qSin(2 * M_PI * sampleRate * qreal(sampleIndex++ % format.sampleRate())
                             / format.sampleRate());
        for (int i = 0; i < format.channelCount(); ++i) {
            switch (format.sampleFormat()) {
            case QAudioFormat::UInt8:
                *reinterpret_cast<quint8 *>(ptr) = static_cast<quint8>((1.0 + x) / 2 * 255);
                break;
            case QAudioFormat::Int16:
                *reinterpret_cast<qint16 *>(ptr) = static_cast<qint16>(x * 32767);
                break;
            case QAudioFormat::Int32:
                *reinterpret_cast<qint32 *>(ptr) =
                    static_cast<qint32>(x * std::numeric_limits<qint32>::max());
                break;
            case QAudioFormat::Float:
                *reinterpret_cast<float *>(ptr) = x;
                break;
            default:
                break;
            }

            ptr += channelBytes;
            length -= channelBytes;
        }
    }
}

qint64 Generator::readData(char *data, qint64 len)
{
    qDebug() << "Generator::readData " << len;
    qint64 total = 0;
    if (!m_buffer.isEmpty()) {
        while (len - total > 0) {
            const qint64 chunk = qMin((m_buffer.size() - m_pos), len - total);
            memcpy(data + total, m_buffer.constData() + m_pos, chunk);
            m_pos = (m_pos + chunk) % m_buffer.size();
            total += chunk;
        }
    }
    return total;
}

qint64 Generator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 Generator::bytesAvailable() const
{
    qDebug() << "Generator::bytesAvailable " << m_buffer.size() + QIODevice::bytesAvailable();
    return m_buffer.size() + QIODevice::bytesAvailable();
}
*/

void AudioOutput::noteOn(int vid, float f)
{
    m_generator->noteOn(vid,f);
}

void AudioOutput::noteOff(int vid)
{
    m_generator->noteOff(vid);
}

void AudioOutput::set_arpeggio_enabled(bool value)
{
    m_generator->getSyctl()->set_arpeggio_enabled(value);
}

void AudioOutput::set_arpeggio_samples(int value)
{
    m_generator->getSyctl()->set_arpeggio_samples(value);
}

void AudioOutput::set_arpeggio_octaves(int value)
{
    m_generator->getSyctl()->set_arpeggio_octaves(value);
}

void AudioOutput::set_arpeggio_step(int value)
{
    synth::Arpeggio::Step step;

    switch(value) {
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

    m_generator->getSyctl()->set_arpeggio_step(step);
}

void AudioOutput::set_filter_cutoff(int value)
{
    m_generator->getSyctl()->set_filter_cutoff(value);
}

void AudioOutput::set_filter_resonance(float value)
{
    m_generator->getSyctl()->set_filter_resonance(value);
}

void AudioOutput::set_osc1_level(float value)
{
    m_generator->getSyctl()->set_osc1_level(value);
}

synth::Oscillator::WaveType AudioOutput::int2wavetype(int value) {
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

synth::Controller::OctaveShift AudioOutput::int2octaveshift(int value) {
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

void AudioOutput::set_osc1_wave_type(int value)
{
    m_generator->getSyctl()->set_osc1_wave_type(int2wavetype(value));
}

void AudioOutput::set_osc1_octave(int octave)
{
    m_generator->getSyctl()->set_osc1_octave(int2octaveshift(octave));
}

void AudioOutput::set_osc2_level(float value)
{
    m_generator->getSyctl()->set_osc2_level(value);
}

void AudioOutput::set_osc2_wave_type(int value)
{
    m_generator->getSyctl()->set_osc2_wave_type(int2wavetype(value));
}

void AudioOutput::set_osc2_octave(int octave)
{
    m_generator->getSyctl()->set_osc2_octave(int2octaveshift(octave));
}

void AudioOutput::set_osc2_shift(int value)
{
    m_generator->getSyctl()->set_osc2_shift(value);
}

void AudioOutput::set_modulation_source(int value)
{
    m_generator->getSyctl()->set_modulation_source(static_cast<synth::Controller::ModulationSource>(value));
}

void AudioOutput::set_modulation_destination(int value)
{
    m_generator->getSyctl()->set_modulation_destination(static_cast<synth::Controller::ModulationDestination>(value));
}

void AudioOutput::set_modulation_amount(float value)
{
    m_generator->getSyctl()->set_modulation_amount(value);
}

void AudioOutput::set_modulation_frequency(float value)
{
    m_generator->getSyctl()->set_modulation_frequency(value);
}

AudioOutput::AudioOutput() : m_devices(new QMediaDevices(this)), m_pushTimer(new QTimer(this))
{
    initializeAudio(m_devices->defaultAudioOutput());
}

AudioOutput::~AudioOutput()
{
    m_pushTimer->stop();
}

void AudioOutput::initializeAudio(const QAudioDevice &deviceInfo)
{
    QAudioFormat format = deviceInfo.preferredFormat();

    const int durationSeconds = 1;
    const int toneSampleRateHz = 600;
    //m_generator.reset(new Generator(format, durationSeconds * 1000000, toneSampleRateHz));
    m_generator.reset(new mobileSynthQT68());
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
void AudioOutput::deviceChanged(int index)
{
    m_generator->stop();
    m_audioOutput->stop();
    m_audioOutput->disconnect(this);
    initializeAudio(m_deviceBox->itemData(index).value<QAudioDevice>());
}
*/

void AudioOutput::volumeChanged(int value)
{
    qreal linearVolume = QAudio::convertVolume(value / qreal(100), QAudio::LogarithmicVolumeScale,
                                               QAudio::LinearVolumeScale);

    m_audioOutput->setVolume(linearVolume);
}

/*
void AudioOutput::updateAudioDevices()
{
    m_deviceBox->clear();
    const QList<QAudioDevice> devices = m_devices->audioOutputs();
    for (const QAudioDevice &deviceInfo : devices)
        m_deviceBox->addItem(deviceInfo.description(), QVariant::fromValue(deviceInfo));
}
*/

void AudioOutput::toggleMode()
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

void AudioOutput::toggleSuspendResume()
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
