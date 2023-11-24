import QtQuick
import QtQuick.Controls

Label {
    property string url

    text: "🌐"

    DefaultToolTip { toolTipText: "Open in browser" }
    HoverHandler { cursorShape: Qt.PointingHandCursor }
    TapHandler { onTapped: Qt.openUrlExternally(url) }
}