#include "audio_recorder.hpp"

#include <QStandardPaths>
#include <QAudioInput>
#include <QImageCapture>
#include <QAudioDevice>
#include <QVariant>
#include <QMediaFormat>
#include <QUrl>

AudioRecorder::AudioRecorder(QObject *parent)
    : QObject(parent)
    , m_filePath("")
    , m_captureSession()
    , m_audioRecorder(nullptr)
{
    m_audioRecorder = new QMediaRecorder(this);
    m_captureSession.setRecorder(m_audioRecorder);
    m_captureSession.setAudioInput(new QAudioInput(this));
}

AudioRecorder::~AudioRecorder()
{
}

QString AudioRecorder::filePath() const
{
    return m_filePath;
}

void AudioRecorder::start()
{
#ifdef Q_OS_ANDROID
    m_audioRecorder->setOutputLocation(QUrl());
#else
    m_audioRecorder->setOutputLocation(QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/intellicute_recording.wav"));
#endif
    m_captureSession.audioInput()->setDevice(QVariant(QString()).value<QAudioDevice>()); // default
    QMediaFormat mediaFormat(QMediaFormat::FileFormat::Wave);
    mediaFormat.setAudioCodec(QMediaFormat::AudioCodec::Wave);
    m_audioRecorder->setMediaFormat(mediaFormat);
    m_audioRecorder->setAudioSampleRate(44100);
    m_audioRecorder->setAudioBitRate(0); // default
    m_audioRecorder->setQuality(QMediaRecorder::NormalQuality);
    m_audioRecorder->setEncodingMode(QMediaRecorder::ConstantQualityEncoding);
    m_audioRecorder->record();
}

void AudioRecorder::stop()
{
    m_audioRecorder->stop();
#ifdef Q_OS_ANDROID
    // for some reason audio recordings are created in this place instead of wathever is configured
    QDir configPath(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/Music");
    QStringList filters = {"*.m4a"};
    configPath.setNameFilters(filters);
    auto fileList = configPath.entryList(QDir::Files, QDir::Name);
    m_filePath = configPath.filePath(fileList.back());
    // TODO cleanup files from previous runs
#else
    m_filePath = m_audioRecorder->actualLocation().toLocalFile();
#endif
    qDebug() << m_filePath << m_audioRecorder->outputLocation();
    emit filePathChanged();
    // delay recording completed to make sure file is written
    QTimer::singleShot(100, this, &AudioRecorder::recordingComplete);
}
