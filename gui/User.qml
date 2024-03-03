import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.platform

RowLayout {
    property var user: null
    property int size: 23
    property bool editable: false

    property alias font: label.font
    property alias showLabel: label.visible

    id: root

    Image {
        id: image
        Layout.maximumHeight: size
        Layout.minimumHeight: size
        Layout.maximumWidth: size
        Layout.minimumWidth: size

        source: user ? user.avatarUrl : ""
        fillMode: Image.PreserveAspectFit
        mipmap: true
        smooth: false

        DefaultToolTip { toolTipText: user ? user.username : "" }
    }

    Label {
        id: label
        Layout.fillWidth: true
        leftPadding: 5
        text: user ? user.username : ""
    }
}
