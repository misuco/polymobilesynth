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

class mobileSynthQT68 : public QIODevice
{
    Q_OBJECT

public:
    mobileSynthQT68();
    ~mobileSynthQT68();

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
