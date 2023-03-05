#pragma once

#include <QObject>
#include <QtQml>

class FileIO : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QUrl filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
    Q_PROPERTY(bool error READ hasError NOTIFY errorChanged)

public:
    explicit FileIO(QObject *parent = nullptr);

public slots:
    bool read();
    bool write();

    QUrl filePath() const;
    void setFilePath(const QUrl &filePath);

    QString text() const;
    void setText(const QString &text);

    QString errorString() const;
    
    bool hasError() const;

signals:
    void filePathChanged();
    void textChanged();
    void errorStringChanged();
    void errorChanged();

private:
    QUrl m_filePath;
    QString m_text;
    QString m_errorString;
    bool m_error;
};
