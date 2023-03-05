#pragma once

#include <QObject>
#include <QtQml>

class OpenAIMessage: public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("OpenAIMessage is not creatable")

    Q_PROPERTY(QString content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(Role role READ role WRITE setRole NOTIFY roleChanged)

    Q_ENUMS(Role)

public:
    enum class Role {
        System,
        User,
        Assistant
    };

    explicit OpenAIMessage(QObject *parent = nullptr);
    explicit OpenAIMessage(const QString& content, Role role, QObject *parent = nullptr);
    ~OpenAIMessage();

    inline static QString roleToString(Role role)
    {
        switch (role) {
            case Role::System:
                return "system";
            case Role::User:
                return "user";
            case Role::Assistant:
                return "assistant";
            default:
                return "unknown";
        }
    }

    inline static Role roleFromString(const QString& role)
    {
        if (role == "system") {
            return Role::System;
        } else if (role == "user") {
            return Role::User;
        } else if (role == "assistant") {
            return Role::Assistant;
        } else {
            return Role::System;
        }
    }


public slots:
    QString content() const;
    void setContent(const QString& content);

    Role role() const;
    void setRole(Role role);

signals:
    void contentChanged();
    void roleChanged();

private:
    QString m_content;
    Role m_role;

};
