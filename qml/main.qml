import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    id: mainWindow

    width: 640
    height: 160
    visible: true
    title: qsTr("Pi Pico USB Interface")

    property int verticalSpacing: mainWindow.height * 0.05

    Item {
        id: entryArea
        anchors.top: parent.top
        anchors.topMargin: verticalSpacing
        anchors.horizontalCenter: parent.horizontalCenter

        width: childrenRect.width
        height: childrenRect.height

        Row {
            TextField {
                id: textField
                width: mainWindow.width * 0.6
                maximumLength: usbInterface.maxCommandLength();
            }

            Button {
                text: qsTr("Send data")

                onClicked: {
                    usbInterface.sendCommand(textField.text);
                }
            }
        }
    }

    Text {
        anchors.top: entryArea.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: verticalSpacing

        text: qsTr("USB response: ") + usbInterface.responseText
    }
}
