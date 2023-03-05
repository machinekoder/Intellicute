#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlPropertyMap>

class JsonMapper : public QObject
{
    Q_OBJECT

public:
    explicit JsonMapper(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE QQmlPropertyMap* mapJson(const QString& filePath) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open file for reading:" << filePath;
            return nullptr;
        }

        QByteArray jsonData = file.readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);

        if (!jsonDoc.isObject()) {
            qWarning() << "JSON data is not an object:" << filePath;
            return nullptr;
        }

        QJsonObject jsonObj = jsonDoc.object();
        QQmlPropertyMap* map = new QQmlPropertyMap();
        for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
            QString key = it.key();
            QVariant value = mapJsonValue(it.value());
            map->insert(key, value);
        }

        return map;
    }

private:
    QVariant mapJsonValue(const QJsonValue& jsonValue) {
        if (jsonValue.isObject()) {
            QJsonObject jsonObj = jsonValue.toObject();
            QQmlPropertyMap* map = new QQmlPropertyMap();
            for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
                QString key = it.key();
                QVariant value = mapJsonValue(it.value());
                map->insert(key, value);
            }
            return QVariant::fromValue(map);
        } else {
            return jsonValue.toVariant();
        }
    }
};