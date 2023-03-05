#pragma once

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QtQml>

class WatsonTTSRequest: public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken NOTIFY accessTokenChanged)
    Q_PROPERTY(QString voice READ voice WRITE setVoice NOTIFY voiceChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QUrl apiURL READ apiURL WRITE setApiURL NOTIFY apiURLChanged)
    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
    Q_PROPERTY(RequestStatus status READ status NOTIFY statusChanged)

    Q_ENUMS(RequestStatus)

public:
    explicit WatsonTTSRequest(QObject *parent = nullptr);
    ~WatsonTTSRequest();

    enum class RequestStatus {
        Idle,
        InProgress,
        Error,
        Success
    };

public slots:
    QString accessToken() const;
    void setAccessToken(const QString& accessToken);

    QString voice() const;
    void setVoice(const QString& voice);

    QString text() const;
    void setText(const QString& text);

    QUrl apiURL() const;
    void setApiURL(const QUrl& apiURL);

    QString filePath() const;
    QString errorString() const;
    RequestStatus status() const;

    void execute();

signals:
    // Signal emitted when the request is finished successfully
    void requestFinished();

    // Signal emitted when the request encounters an error
    void requestError(const QString& errorString);

    void accessTokenChanged();
    void voiceChanged();
    void textChanged();
    void apiURLChanged();
    void filePathChanged();
    void errorStringChanged();
    void statusChanged();

private:
    QNetworkAccessManager *m_networkAccessManager;
    QString m_accessToken;
    QString m_voice;
    QString m_text;
    QUrl m_apiURL;
    QString m_filePath;
    QString m_errorString;
    RequestStatus m_status;

    void sendRequest();
};
