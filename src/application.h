#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "usbinterface.h"

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QObject *parent = nullptr);
    virtual ~Application();

public slots:
    // initialize and start application
    int run(int argc, char *argv[]);

signals:

private:
    // GUI handlers
    QGuiApplication *m_pApp;
    QQmlApplicationEngine *m_pEngine;

    // USB interface to Pi Pico
    USBInterface *m_pUSBInterface;
};

#endif // APPLICATION_H
