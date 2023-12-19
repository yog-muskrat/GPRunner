import QtQuick
import QtQuick.Controls

Label {
    property string url
    property string toolTip: "Open in browser"

    text: "🌐"

    DefaultToolTip { toolTipText: toolTip }
    HoverHandler { cursorShape: Qt.PointingHandCursor }
    TapHandler { onTapped: Qt.openUrlExternally(url) }
}