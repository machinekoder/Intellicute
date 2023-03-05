#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <QtQml>

#include "openai_message.h"

class OpenAIMessageModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QList<OpenAIMessage*> messages READ messages WRITE setMessages NOTIFY messagesChanged)
    Q_PROPERTY(QString jsonString READ jsonString WRITE setJsonString NOTIFY jsonStringChanged)

public:
    enum Role {
        MessageRole = Qt::UserRole + 1,
        RoleRole
    };

    explicit OpenAIMessageModel(QObject *parent = nullptr);
    ~OpenAIMessageModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    QList<OpenAIMessage *> messages() const;
    void setMessages(const QList<OpenAIMessage *> &newMessages);

    QString jsonString() const;
    void setJsonString(const QString &newJsonString);

public slots:
    void addMessage(OpenAIMessage::Role role, const QString& content);
    void addMessage(OpenAIMessage *message);
    void removeMessage(OpenAIMessage *message);
    void clearMessages();

signals:
    void messagesChanged();
    void jsonStringChanged();

private:
    QList<OpenAIMessage*> m_messages;
    QString m_jsonString;

    QList<OpenAIMessage*> messagesFromJson(const QString &jsonString) const;
    QString jsonFromMessages(const QList<OpenAIMessage*> &messages) const;
};
