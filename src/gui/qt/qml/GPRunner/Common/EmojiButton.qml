import QtQuick
import QtQuick.Controls

Label {
    required property var modelData

    text: modelData.reaction.moji
    padding: 3

    horizontalAlignment: Text.AlignHCenter
    background: Rectangle {
        color: modelData.user == gpm.currentUser ? palette.shadow : palette.button
        radius: 3
        border.width: 1
        border.color: palette.window
    }

    DefaultToolTip { toolTipText: modelData.reaction.name + " by " + modelData.user.username + " at " + modelData.created }

    HoverHandler { cursorShape: Qt.PointingHandCursor }
}
