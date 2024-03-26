import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.platform

Item {
    property var user: null
    property int size: 23
    property bool editable: false

    property alias font: label.font
    property alias showLabel: label.visible

    implicitWidth: mainLayout.implicitWidth
    implicitHeight: size

    signal userSelected(var newUser)
    signal userCleared()

    id: root

    RowLayout {
        id: mainLayout

        spacing: 5

        Rectangle {
            Layout.maximumHeight: size
            Layout.minimumHeight: size
            Layout.maximumWidth: size
            Layout.minimumWidth: size
    
            border.color: palette.light
            border.width: imageHover.hovered ? 1 : 0

            color: "transparent"

            Image {
                id: image
                
                anchors.fill: parent
                anchors.margins: 1
                source: user && user.avatarUrl ? user.avatarUrl : ""
                fillMode: Image.PreserveAspectFit
                mipmap: true
                smooth: false
    
            }

            DefaultToolTip { toolTipText: user ? user.username : "" }

            HoverHandler {
                id: imageHover
                enabled: editable
                cursorShape: Qt.PointingHandCursor
            }

            TapHandler {
                enabled: editable
                onTapped: { combo.popup.open() }
            }
        }

        Label {
            id: clear

            visible: editable && user

            text: "X"

            DefaultToolTip { toolTipText: "Clear user" }

            HoverHandler { cursorShape: Qt.PointingHandCursor }

            TapHandler {
                onTapped: {
                    root.user = null
                    root.userCleared()
                }
            }
        }

        Label {
            id: label
            Layout.fillWidth: true
            
            text: user ? user.username : ""
        }
    }

    ComboBox {
        id: combo

        visible: popup.opened

        background: Pane{}
        model: gpm.activeUsers
        textRole: "username"

        currentIndex: indexOfValue(user)

        implicitContentWidthPolicy: ComboBox.WidestText
        onActivated: (idx) => {
            root.user = currentValue
            combo.popup.close()
            root.userSelected(root.user)
        }
    }
}
