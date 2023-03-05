#pragma once
#include <QObject>
#include <QMediaRecorder>
#include <QMediaCaptureSession>
#include <QUrl>
#include <QtQml>


class AudioRecorder: public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)

    public:
        explicit AudioRecorder(QObject *parent = nullptr);
        virtual ~AudioRecorder();

    public slots:
        QString filePath() const;

        void start();
        void stop();
    
    signals:
        void filePathChanged();
        void recordingComplete();

    private:
        QString m_filePath;
        QMediaCaptureSession m_captureSession;
        QMediaRecorder *m_audioRecorder = nullptr;
};
