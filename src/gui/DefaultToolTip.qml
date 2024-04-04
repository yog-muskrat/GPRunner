import QtQuick
import QtQuick.Controls

Item {
    property string toolTipText

    anchors.fill: parent

    ToolTip.delay: 500
    ToolTip.timeout: 3000
    ToolTip.text: toolTipText
    ToolTip.visible: hover.hovered

    HoverHandler {
        id: hover
        enabled: toolTipText
    }
}
