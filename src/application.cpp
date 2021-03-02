#include "application.h"

Application::Application(QObject *parent) : QObject(parent)
{

}

Application::~Application()
{
    delete m_pUSBInterface;
    delete m_pEngine;
    delete m_pApp;
}

int Application::run(int argc, char *argv[])
{
    // set high dpi scaling
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // init app
    m_pApp = new QGuiApplication(argc, argv);
    m_pEngine = new QQmlApplicationEngine();

    // create USB interface
    m_pUSBInterface = new USBInterface(0xcafe, 0x4004, this);

    // connect USBInterface to the GUI
    m_pEngine->rootContext()->setContextProperty("usbInterface", m_pUSBInterface);

    // load the base QML file
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(m_pEngine, &QQmlApplicationEngine::objectCreated,
                     m_pApp, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    m_pEngine->load(url);

    // run the application
    return m_pApp->exec();
}
