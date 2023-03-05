#include "fileio.h"
#include <QFile>

FileIO::FileIO(QObject *parent) 
    : QObject(parent)
    , m_error(false)
{
}

bool FileIO::read()
{
    QString filePath;
    if (m_filePath.isLocalFile()) {
        filePath = m_filePath.toLocalFile();
    }
    else {
        filePath = m_filePath.toString();
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_error = true;
        m_errorString = tr("Could not open file for reading");
        emit errorStringChanged();
        return false;
    }
    else {
        m_error = false;
        m_errorString = QString();
        emit errorStringChanged();
    }

    m_text = QString::fromUtf8(file.readAll());
    file.close();

    emit textChanged();
    return true;
}

bool FileIO::write()
{
    QString filePath;
    if (m_filePath.isLocalFile()) {
        filePath = m_filePath.toLocalFile();
    }
    else {
        filePath = m_filePath.toString();
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_error = true;
        m_errorString = tr("Could not open file for writing");
        emit errorStringChanged();
        return false;
    }
    else {
        m_error = false;
        m_errorString = QString();
        emit errorStringChanged();
    }

    file.write(m_text.toUtf8());
    file.close();

    return true;
}

QUrl FileIO::filePath() const
{
    return m_filePath;
}

void FileIO::setFilePath(const QUrl &filePath)
{
    if (m_filePath == filePath)
        return;

    m_filePath = filePath;
    emit filePathChanged();
}

QString FileIO::text() const
{
    return m_text;
}

void FileIO::setText(const QString &text)
{
    if (m_text == text)
        return;

    m_text = text;
    emit textChanged();
}

QString FileIO::errorString() const
{
    return m_errorString;
}

bool FileIO::hasError() const
{
    return m_error;
}
