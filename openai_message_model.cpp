#include "openai_message_model.h"

OpenAIMessageModel::OpenAIMessageModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(this, &OpenAIMessageModel::messagesChanged, this, &OpenAIMessageModel::jsonStringChanged);
}

OpenAIMessageModel::~OpenAIMessageModel()
{
}

int OpenAIMessageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_messages.count();
}

QVariant OpenAIMessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    OpenAIMessage *message = m_messages.at(index.row());

    switch (role) {
    case MessageRole:
        return QVariant::fromValue(message);
    case RoleRole:
        return QVariant::fromValue(message->role());
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> OpenAIMessageModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MessageRole] = "message";
    roles[RoleRole] = "role";
    return roles;
}

void OpenAIMessageModel::addMessage(OpenAIMessage *message)
{
    beginInsertRows(QModelIndex(), m_messages.count(), m_messages.count());
    m_messages.append(message);
    endInsertRows();
    emit messagesChanged();
}

void OpenAIMessageModel::addMessage(OpenAIMessage::Role role, const QString &content)
{
    OpenAIMessage *message = new OpenAIMessage(content, role);
    addMessage(message);
}

void OpenAIMessageModel::removeMessage(OpenAIMessage *message)
{
    int pos = m_messages.indexOf(message);
    if (pos != -1) {
        beginRemoveRows(QModelIndex(), pos, pos);
        m_messages.removeAt(pos);
        endRemoveRows();
        emit messagesChanged();
        message->deleteLater();
    }
}

void OpenAIMessageModel::clearMessages()
{
    beginResetModel();
    for (OpenAIMessage *message : m_messages) {
        message->deleteLater();
    }
    m_messages.clear();
    endResetModel();
    emit messagesChanged();
}

QList<OpenAIMessage *> OpenAIMessageModel::messagesFromJson(const QString &jsonString) const
{
    QList<OpenAIMessage *> messages;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8());
    if (jsonDocument.isNull()) {
        qWarning() << "Failed to read JSON document";
        return messages;
    }
    // read "messages" from json object
    QJsonObject jsonObject = jsonDocument.object();
    if (!jsonObject.contains("messages")) {
        qWarning() << "JSON document does not contain 'messages' key";
        return messages;
    }
    QJsonArray jsonArray = jsonObject["messages"].toArray();
    for (const QJsonValue &jsonValue : jsonArray) {
        QJsonObject messageObject = jsonValue.toObject();
        if (!messageObject.contains("role") || !messageObject.contains("content")) {
            qWarning() << "JSON document does not contain 'role' or 'content' key";
            continue;
        }
        OpenAIMessage *message = new OpenAIMessage(
            messageObject["content"].toString(),
            OpenAIMessage::roleFromString(messageObject["role"].toString())
            );
        messages.append(message);
    }
    return messages;
}

QString OpenAIMessageModel::jsonFromMessages(const QList<OpenAIMessage*> &messages) const
{
    QJsonArray jsonArray;
    for (OpenAIMessage *message : messages) {
        QJsonObject messageObject;
        messageObject["role"] = OpenAIMessage::roleToString(message->role());
        messageObject["content"] = message->content();
        jsonArray.append(messageObject);
    }
    QJsonObject jsonObject;
    jsonObject["messages"] = jsonArray;
    QJsonDocument jsonDocument(jsonObject);
    return jsonDocument.toJson(QJsonDocument::Indented);
}

QList<OpenAIMessage *> OpenAIMessageModel::messages() const
{
    return m_messages;
}

void OpenAIMessageModel::setMessages(const QList<OpenAIMessage *> &newMessages)
{
    if (m_messages == newMessages)
        return;
    beginResetModel();
    m_messages = newMessages;
    endResetModel();
    emit messagesChanged();
}

QString OpenAIMessageModel::jsonString() const
{
    return jsonFromMessages(m_messages);
}

void OpenAIMessageModel::setJsonString(const QString &newJsonString)
{
    setMessages(messagesFromJson(newJsonString));
}
