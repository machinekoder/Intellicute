#include "openai_message.h"

OpenAIMessage::OpenAIMessage(QObject *parent):
    QObject(parent)
{
    
}

OpenAIMessage::OpenAIMessage(const QString& content, OpenAIMessage::Role role, QObject *parent):
    QObject(parent),
    m_content(content),
    m_role(role)
{
    
}


OpenAIMessage::~OpenAIMessage()
{

}


QString OpenAIMessage::content() const
{
    return m_content;
}

void OpenAIMessage::setContent(const QString& content)
{
    if (m_content != content) {
        m_content = content;
        emit contentChanged();
    }
}

OpenAIMessage::Role OpenAIMessage::role() const
{
    return m_role;
}

void OpenAIMessage::setRole(OpenAIMessage::Role role)
{
    if (m_role != role) {
        m_role = role;
        emit roleChanged();
    }
}
