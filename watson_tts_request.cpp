#include "watson_tts_request.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>


WatsonTTSRequest::WatsonTTSRequest(QObject *parent) 
    : QObject(parent)
    , m_networkAccessManager(new QNetworkAccessManager(this))
    , m_accessToken("")
    , m_voice("")
    , m_text("")
    , m_apiURL("")
    , m_filePath("")
    , m_errorString("")
    , m_status(RequestStatus::Idle)
{
    m_filePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/watson_tts.ogg";
}

WatsonTTSRequest::~WatsonTTSRequest()
{

}

void WatsonTTSRequest::sendRequest()
{
    // Set up the API endpoint URL
    QUrl endpointUrl = m_apiURL.resolved(QUrl("v1/synthesize"));

    QUrlQuery params;
    params.addQueryItem("voice", m_voice);
    endpointUrl.setQuery(params);

    // Set up the request headers
    QNetworkRequest request(endpointUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    //request.setRawHeader("Authorization", "Bearer " + m_accessToken.toUtf8());
    request.setRawHeader("Authorization", "Basic " + QString("apikey:" + m_accessToken).toLocal8Bit().toBase64());
    //request.setRawHeader("Accept", "audio/wav");
    request.setRawHeader("Accept", "audio/ogg;codecs=vorbis");

    // Set up the request body
    QJsonObject requestBody;
    requestBody.insert("text", m_text);

    // Send the request
    QNetworkReply *reply = m_networkAccessManager->post(request, QJsonDocument(requestBody).toJson(QJsonDocument::Compact));
    m_status = RequestStatus::InProgress;
    emit statusChanged();

    // Connect signals and slots to handle the response
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QFile saveFile(m_filePath);
            if (saveFile.open(QIODevice::WriteOnly)) {
                saveFile.write(reply->readAll());
                saveFile.close();
                m_status = RequestStatus::Success;
                emit statusChanged();
                emit requestFinished();
            } else {
                m_errorString = "Could not open file for writing";
                emit errorStringChanged();
                m_status = RequestStatus::Error;
                emit statusChanged();
                emit requestError(m_errorString);
            }
        } else {
            m_errorString = reply->errorString();
            qDebug() << reply->error() << reply->errorString() << reply->readAll();
            emit errorStringChanged();
            m_status = RequestStatus::Error;
            emit statusChanged();
            emit requestError(m_errorString);
        }

        reply->deleteLater();
    });
}

void WatsonTTSRequest::execute()
{
    sendRequest();
}

QString WatsonTTSRequest::accessToken() const
{
    return m_accessToken;
}

void WatsonTTSRequest::setAccessToken(const QString& accessToken)
{
    if (m_accessToken != accessToken) {
        m_accessToken = accessToken;
        emit accessTokenChanged();
    }
}

QString WatsonTTSRequest::voice() const
{
    return m_voice;
}

void WatsonTTSRequest::setVoice(const QString& voice)
{
    if (m_voice != voice) {
        m_voice = voice;
        emit voiceChanged();
    }
}

QString WatsonTTSRequest::text() const
{
    return m_text;
}

void WatsonTTSRequest::setText(const QString& text)
{
    if (m_text != text) {
        m_text = text;
        emit textChanged();
    }
}

QUrl WatsonTTSRequest::apiURL() const
{
    return m_apiURL;
}

void WatsonTTSRequest::setApiURL(const QUrl& apiURL)
{
    if (m_apiURL != apiURL) {
        m_apiURL = apiURL;
        emit apiURLChanged();
    }
}

QString WatsonTTSRequest::filePath() const
{
    return m_filePath;
}

QString WatsonTTSRequest::errorString() const
{
    return m_errorString;
}

WatsonTTSRequest::RequestStatus WatsonTTSRequest::status() const
{
    return m_status;
}
