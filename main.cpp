#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>
#include <QFile>
#include <QIcon>
#include <QFontDatabase>


int main(int argc, char *argv[])
{
#ifdef QT_DEBUG
    qDebug() << qgetenv("LD_LIBRARY_PATH");
#endif

    QGuiApplication app(argc, argv);
    app.setOrganizationName(u"Machine Koder"_qs);
    app.setOrganizationDomain(u"machinekoder.com"_qs);
    app.setApplicationName(u"Intellicute"_qs);
    app.setApplicationVersion(u"1.0"_qs);

    // set icon from resource
    app.setWindowIcon(QIcon(u":/icons/intellicute_512.png"_qs));

    QCommandLineParser parser;
    parser.setApplicationDescription("Intellicute, a Qt AI Assistant");
    parser.addHelpOption();
#ifdef QT_DEBUG
    QCommandLineOption forceOption(QStringList() << "l" << "live",
    QCoreApplication::translate("live", "Start live coding mode."));
    parser.addOption(forceOption);
#endif
    parser.process(app);

    QString fileName = u"main.qml"_qs;
    #ifdef QT_DEBUG
        if (parser.isSet(forceOption)) {
            fileName = u"live.qml"_qs;
        }
    #endif

    if (QFontDatabase::addApplicationFont(":/fonts/fa-solid-900.ttf") == -1) {
        qWarning() << "Failed to load fonts";
    }

    QQmlApplicationEngine engine;
    QFile file(u"./"_qs + fileName);
    QUrl url;
    if (file.exists()) {
        url = u"./"_qs + fileName;
        qputenv("QT_QUICK_CONTROLS_CONF", "qtquickcontrols2.conf");
    }
    else {
        engine.addImportPath(u"qrc:/"_qs);
        url = u"qrc:/"_qs + fileName;
        qputenv("QT_QUICK_CONTROLS_CONF", ":/qtquickcontrols2.conf");
    }
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
