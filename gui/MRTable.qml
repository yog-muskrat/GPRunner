import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "Utility.js" as Utility

Item {
    property int currentMR: 0

    clip: true

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        HorizontalHeaderView {
            id: header
            syncView: mrs
            Layout.fillWidth: true
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            TableView {
                id: mrs
                anchors.fill: parent

                focus: true
                clip: true

                model: gpm.mrModel
                selectionModel: ItemSelectionModel {
                }

                columnWidthProvider: Utility.calcColumnWidth.bind(this, header)

                delegate: Rectangle {
                    implicitWidth: itemText.implicitWidth + urlButton.implicitWidth
                    implicitHeight: itemText.implicitHeight
                    color: row == mrs.currentRow ? palette.highlight : ((row % 2) == 0 ? palette.alternateBase : palette.base)

                    Text {
                        id: urlButton

                        visible: column == 0
                        padding: 5
                        text: "🌐"

                        HoverHandler { cursorShape: Qt.PointingHandCursor }
                        TapHandler { onTapped: Qt.openUrlExternally(model.url) }
                    }

                    Text {
                        id: itemText

                        anchors.fill: parent
                        anchors.leftMargin: urlButton.visible ? urlButton.width - 5 : 0
                        anchors.rightMargin: unreadIndicator.visible ? unreadIndicator.width : 0
                        padding: 5
                        text: model.display
                        font.bold: model.font.bold
                        color: mrs.currentRow ? palette.highlightedText : palette.text
                        ToolTip.delay: 500
                        ToolTip.timeout: 3000
                        ToolTip.text: model.toolTip
                        ToolTip.visible: model.toolTip ? hoverHandler.hovered : false

                        HoverHandler { id: hoverHandler }
                        TapHandler { onTapped: currentMR = model.id }
                    }

                    Rectangle {
                        id: unreadIndicator

                        anchors.right: parent.right
                        anchors.rightMargin: 5
                        anchors.verticalCenter: itemText.verticalCenter
                        visible: model.hasUnreadNotes

                        width: itemText.implicitHeight / 3
                        height: width
                        radius: width / 2
                        color: "#B21818"
                    }
                }
            }
        }
    }
}
