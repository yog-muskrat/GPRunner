import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.platform

RowLayout {
    property var user: null
    property int size: 28

    property bool boldFont: false

    id: root

    Image {
        Layout.maximumHeight: size
        Layout.minimumHeight: size
        Layout.maximumWidth: size
        Layout.minimumWidth: size

        source: user ? user.avatarUrl : ""
        fillMode: Image.PreserveAspectFit
    }

    Label {
        Layout.fillWidth: true
        leftPadding: 5
        text: user ? user.username : ""
        font.bold: root.boldFont
    }
}
