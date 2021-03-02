#include "usbinterface.h"

USBInterface::USBInterface(const unsigned short& vid, const unsigned short& pid, QObject *parent) : QObject(parent)
{
    // specify the expected VID, PID
    m_VID = vid;
    m_PID = pid;

    handle = nullptr;
}

USBInterface::~USBInterface()
{
    // close the connection the device
    hid_close(handle);

    // Free static HIDAPI objects.
    hid_exit();
}

bool USBInterface::init()
{
    int res;
    unsigned char buf[MAX_BUF];
    wchar_t wstr[MAX_STR];

    struct hid_device_info *devs, *cur_dev;

    // check if handle is already open
    // TODO - more robust checking if USB is actually ready and connected
    if(handle != nullptr)
    {
        return true;
    }

    // init HID
    if (hid_init())
    {
        qDebug() << "Failed to init HID";
        return false;
    }

    // find all HID devices, iterate through to find our device
    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    while (cur_dev)
    {
        // match VID, display specifications
        if(cur_dev->vendor_id == m_VID)
        {
            printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
            printf("\n");
            printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
            printf("  Product:      %ls\n", cur_dev->product_string);
            printf("  Release:      %hx\n", cur_dev->release_number);
            printf("  Interface:    %d\n",  cur_dev->interface_number);
            printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
            printf("\n");
        }
        cur_dev = cur_dev->next;
    }

    // release enumeration
    hid_free_enumeration(devs);

    // Set up the command buffer.
    memset(buf,0x00,sizeof(buf));

    // Open the device using the VID, PID,
    // and optionally the Serial number.
    handle = hid_open(m_VID, m_PID, NULL);

    // Read the Manufacturer String
    wstr[0] = 0x0000;
    res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
    if (res < 0)
        printf("Unable to read manufacturer string\n");
    printf("Manufacturer String: %ls\n", wstr);

    // Read the Product String
    wstr[0] = 0x0000;
    res = hid_get_product_string(handle, wstr, MAX_STR);
    if (res < 0)
        printf("Unable to read product string\n");
    printf("Product String: %ls\n", wstr);

    // Read the Serial Number String
    wstr[0] = 0x0000;
    res = hid_get_serial_number_string(handle, wstr, MAX_STR);
    if (res < 0)
        printf("Unable to read serial number string\n");
    printf("Serial Number String: (%d) %ls", wstr[0], wstr);
    printf("\n");

    // Read Indexed String 1
    wstr[0] = 0x0000;
    res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
    if (res < 0)
        printf("Unable to read indexed string 1\n");
    printf("Indexed String 1: %ls\n", wstr);

    // Set the hid_read() function to be non-blocking.
    hid_set_nonblocking(handle, 1);

    // Try to read from the device. There should be no
    // data here, but execution should not block.
    res = hid_read(handle, buf, 1);

    return true;
}

bool USBInterface::sendCommand(const QString& sCommand)
{
    // make sure the command fits in the buffer
    if(sCommand.length() > MAX_COMMAND)
    {
        qDebug() << "Command larger than buffer, not sending";
        return -1;
    }

    int res;
    unsigned char buf[MAX_BUF];

    // make sure USB is init, otherwise attempt init
    if (!init())
    {
        printf("unable to open device\n");
        return false;
    }

    // Try to read from the device. There should be no
    // data here, but execution should not block.
    res = hid_read(handle, buf, 1);

    // Send data to the device
    qDebug() << "Sending command:" << sCommand;
    hid_write(handle, (unsigned char*)sCommand.toStdString().c_str(), sCommand.length());
    if (res < 0)
    {
        qDebug() << "Failed to write command";
        return false;
    }

    // Read requested state. hid_read() has been set to be
    // non-blocking by the call to hid_set_nonblocking() above.
    // This loop demonstrates the non-blocking nature of hid_read().
    res = 0;
    while (res == 0)
    {
        // attempt to read from USB device
        res = hid_read(handle, buf, sizeof(buf));

        // if empty
        if (res == 0)
        {
            printf("waiting...\n");
        }

        // if error
        if (res < 0)
        {
            printf("Unable to read()\n");
            return false;
        }

        // small wait
        usleep(5000);
    }

    // Print out the returned buffer.
    printf("Data read:\n   ");
    for (int i = 0; i < res; i++)
        printf("%02hhx ", buf[i]);
    printf("\n");

    // pass the response to the ui
    QString sResponse = QString::fromStdString((char *)buf);
    setLastResponse(sResponse);

    return 0;
}
