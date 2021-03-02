#ifndef USBINTERFACE_H
#define USBINTERFACE_H

#include <QObject>
#include <QDebug>

#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include "hidapi.h"
#include <unistd.h>

#define MAX_STR 255
#define MAX_BUF 512
#define MAX_COMMAND 64

class USBInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString responseText READ getLastResponse WRITE setLastResponse NOTIFY signal_newResponse)

public:
    explicit USBInterface(const unsigned short& vid, const unsigned short& pid, QObject *parent = nullptr);
    virtual ~USBInterface();

public slots:
    // initialize and open the usb connection
    bool init();

    // send the command to the device
    bool sendCommand(const QString &sCommand);

    // expose max command length to the ui
    int maxCommandLength() { return MAX_COMMAND; }

    // getter/setter for handling USB response
    QString getLastResponse() const { return m_sLastResponse; }
    void setLastResponse(const QString& sResponse) {
        if(m_sLastResponse != sResponse) {
            m_sLastResponse = sResponse;
            emit signal_newResponse(m_sLastResponse);
        }
    }

signals:
    // notify ui/property of new USB response
    void signal_newResponse(const QString& sResponse);

private:
    // most recent response from USB, assume QString for simplicity
    QString m_sLastResponse;

    // handle to the usb interface
    hid_device *handle;

    // specified VID/PID of expected USB device
    unsigned short m_VID;
    unsigned short m_PID;

};

#endif // USBINTERFACE_H
