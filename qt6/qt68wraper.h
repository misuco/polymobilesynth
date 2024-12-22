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

    synth::Controller * getSyctl() {return syctl;}

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

    synth::Controller * syctl;
    QAudioSink *m_audioOutput;
    QIODevice *m_output; // not owned
    QAudioFormat m_format;
    //QAudioDevice m_device;

    QTimer sampleTimer;

};

#endif // QT68WRAPER_H
#endif // Q_OS_IOS
