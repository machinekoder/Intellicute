#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QHttpMultiPart>
#include <QHttpPart>

#include "openai_request.hpp"

OpenAIRequest::OpenAIRequest(QObject *parent)
    : QObject(parent)
    , m_networkAccessManager(new QNetworkAccessManager(this))
    , m_accessToken("")
    , m_model("")
    , m_prompt("")
    , m_filePath("")
    , m_generatedText("")
    , m_errorString("")
    , m_status(RequestStatus::Idle)
    , m_maxTokens(100)
    , m_temperature(0.3)
    , m_topP(1.0)
    , m_frequencyPenalty(0.0)
    , m_presencePenalty(0.0)
{
    connect(this, &OpenAIRequest::requestFinished, this, [this](const QString& generatedText) {
        m_generatedText = generatedText;
        emit generatedTextChanged();
        m_status = RequestStatus::Success;
        emit statusChanged();
    });
    connect(this, &OpenAIRequest::requestError, this, [this](const QString& errorString) {
        m_errorString = errorString;
        emit errorStringChanged();
        m_status = RequestStatus::Error;
        emit statusChanged();
    });
}

OpenAIRequest::~OpenAIRequest()
{
    for (OpenAIMessage *message : m_messages) {
        message->deleteLater();
    }
    m_messages.clear();
}

void OpenAIRequest::execute()
{
    if (m_model.startsWith("whisper")) {
        sendMultiPartRequest();
    }
    else if (m_model.startsWith("gpt-3")) {
        sendChatRequest();
    }
    else {
        sendRequest();
    }
}

// Send a request to the OpenAI API
void OpenAIRequest::sendRequest()
{
    // Set up the API endpoint URL
    const QUrl endpointUrl("https://api.openai.com/v1/completions");

    // Set up the request headers
    QNetworkRequest request(endpointUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_accessToken).toUtf8());

    // Set up the request body
    QJsonObject requestBody;
    requestBody.insert("model", m_model);
    requestBody.insert("prompt", m_prompt);
    requestBody.insert("max_tokens", m_maxTokens);
    requestBody.insert("temperature", m_temperature);
    requestBody.insert("top_p", m_topP);
    requestBody.insert("frequency_penalty", m_frequencyPenalty);
    requestBody.insert("presence_penalty", m_presencePenalty);

    QJsonDocument requestBodyJson(requestBody);
    QByteArray requestBodyBytes = requestBodyJson.toJson();

    // Send the request
    QNetworkReply *reply = m_networkAccessManager->post(request, requestBodyBytes);
    m_status = RequestStatus::InProgress;
    emit statusChanged();

    // Connect signals and slots to handle the response
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            const QByteArray responseBytes = reply->readAll();
            const QJsonDocument responseJson = QJsonDocument::fromJson(responseBytes);

            // Handle the response
            const QString generatedText = responseJson.object().value("choices").toArray().at(0).toObject().value("text").toString();
            emit requestFinished(generatedText);
        } else {
            emit requestError(reply->errorString());
        }

        reply->deleteLater();
    });
}

void OpenAIRequest::sendChatRequest()
{
    // Set up the API endpoint URL
    const QUrl endpointUrl("https://api.openai.com/v1/chat/completions");

    // Set up the request headers
    QNetworkRequest request(endpointUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_accessToken).toUtf8());

    // Set up the request body
    QJsonObject requestBody;
    requestBody.insert("model", m_model);
    requestBody.insert("max_tokens", m_maxTokens);
    requestBody.insert("temperature", m_temperature);
    requestBody.insert("top_p", m_topP);
    requestBody.insert("frequency_penalty", m_frequencyPenalty);
    requestBody.insert("presence_penalty", m_presencePenalty);
    QJsonArray messageArray;
    auto addMessageArray = [this, &messageArray](OpenAIMessage* message) {
        if (message->content().isEmpty())
            return;
        QJsonObject messageObject;
        messageObject.insert("role", OpenAIMessage::roleToString(message->role()));
        messageObject.insert("content", message->content());
        messageArray.append(messageObject);
    };
    for (OpenAIMessage *message : m_messages) {
        addMessageArray(message);
    }
    requestBody.insert("messages", messageArray);

    QJsonDocument requestBodyJson(requestBody);
    QByteArray requestBodyBytes = requestBodyJson.toJson();

    // Send the request
    QNetworkReply *reply = m_networkAccessManager->post(request, requestBodyBytes);
    m_status = RequestStatus::InProgress;
    emit statusChanged();

    // Connect signals and slots to handle the response
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            const QByteArray responseBytes = reply->readAll();
            const QJsonDocument responseJson = QJsonDocument::fromJson(responseBytes);

            // Handle the response
            const auto message = responseJson.object().value("choices").toArray().at(0).toObject().value("message").toObject();
            //addMessage(OpenAIMessage::roleFromString(message.value("role").toString()), message.value("content").toString());
            emit requestFinished(message.value("content").toString());
        } else {
            emit requestError(reply->errorString() + reply->readAll());
        }

        reply->deleteLater();
    });
}

void OpenAIRequest::sendMultiPartRequest()
{
    // Set up the API endpoint URL
    const QUrl endpointUrl("https://api.openai.com/v1/audio/transcriptions");

    // Set up the request headers
    QNetworkRequest request(endpointUrl);
    request.setRawHeader("Authorization", ("Bearer " + m_accessToken).toUtf8());

    // Set up the form data
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
 
    // Add a JSON parameter
    QHttpPart modelPart;
    modelPart.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    modelPart.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"model\"");
    modelPart.setBody(m_model.toUtf8());

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, "audio/wav");
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"file\"; filename=\"audio.wav\"");
    QFile *file = new QFile(m_filePath);
    file->open(QIODevice::ReadOnly);
    filePart.setBodyDevice(file);
    file->setParent(multiPart); // Ownership transfer
    
    multiPart->append(filePart);
    multiPart->append(modelPart);

    // Send the request
    QNetworkReply *reply = m_networkAccessManager->post(request, multiPart);
    multiPart->setParent(reply); // Ownership transfer
    m_status = RequestStatus::InProgress;
    emit statusChanged();

    // Connect signals and slots to handle the response
    connect(reply, &QNetworkReply::finished, this, [this, file, reply]() {
        file->close();
        if (reply->error() == QNetworkReply::NoError) {
            const QByteArray responseBytes = reply->readAll();
            const QJsonDocument responseJson = QJsonDocument::fromJson(responseBytes);

            // Handle the response
            const QString generatedText = responseJson.object().value("text").toString();
            emit requestFinished(generatedText);
        } else {
            qDebug() << reply->errorString();
            emit requestError(reply->errorString());
        }

        reply->deleteLater();
    });
}

QString OpenAIRequest::accessToken() const
{
    return m_accessToken;
}

void OpenAIRequest::setAccessToken(const QString& accessToken)
{
    if (m_accessToken == accessToken)
        return;
    m_accessToken = accessToken;
    emit accessTokenChanged();
}

QString OpenAIRequest::model() const
{
    return m_model;
}

void OpenAIRequest::setModel(const QString& model)
{
    if (m_model == model)
        return;
    m_model = model;
    emit modelChanged();
}

QString OpenAIRequest::prompt() const
{
    return m_prompt;
}

void OpenAIRequest::setPrompt(const QString& prompt)
{
    if (m_prompt == prompt)
        return;
    m_prompt = prompt;
    emit promptChanged();
}

QString OpenAIRequest::filePath() const
{
    return m_filePath;
}

void OpenAIRequest::setFilePath(const QString& filePath)
{
    if (m_filePath == filePath)
        return;
    m_filePath = filePath;
    emit filePathChanged();
}

QString OpenAIRequest::generatedText() const
{
    return m_generatedText;
}

QString OpenAIRequest::errorString() const
{
    return m_errorString;
}

OpenAIRequest::RequestStatus OpenAIRequest::status() const
{
    return m_status;
}

int OpenAIRequest::maxTokens() const
{
    return m_maxTokens;
}

void OpenAIRequest::setMaxTokens(int maxTokens)
{
    if (m_maxTokens == maxTokens)
        return;
    m_maxTokens = maxTokens;
    emit maxTokensChanged();
}

double OpenAIRequest::temperature() const
{
    return m_temperature;
}

void OpenAIRequest::setTemperature(double temperature)
{
    if (qFuzzyCompare(m_temperature, temperature))
        return;
    m_temperature = temperature;
    emit temperatureChanged();
}

double OpenAIRequest::topP() const
{
    return m_topP;
}

void OpenAIRequest::setTopP(double topP)
{
    if (qFuzzyCompare(m_topP, topP))
        return;
    m_topP = topP;
    emit topPChanged();
}

double OpenAIRequest::frequencyPenalty() const
{
    return m_frequencyPenalty;
}

void OpenAIRequest::setFrequencyPenalty(double frequencyPenalty)
{
    if (qFuzzyCompare(m_frequencyPenalty, frequencyPenalty))
        return;
    m_frequencyPenalty = frequencyPenalty;
    emit frequencyPenaltyChanged();
}

double OpenAIRequest::presencePenalty() const
{
    return m_presencePenalty;
}

void OpenAIRequest::setPresencePenalty(double presencePenalty)
{
    if (qFuzzyCompare(m_presencePenalty, presencePenalty))
        return;
    m_presencePenalty = presencePenalty;
    emit presencePenaltyChanged();
}

QList<OpenAIMessage*> OpenAIRequest::messages() const
{
    return m_messages;
}

void OpenAIRequest::setMessages(const QList<OpenAIMessage *> &newMessages)
{
    if (m_messages == newMessages)
        return;
    m_messages = newMessages;
    emit messagesChanged();
}
