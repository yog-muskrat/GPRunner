import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        HorizontalHeaderView {
            id: header
            syncView: mrs
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth:  true
            Layout.verticalStretchFactor: 1

            TableView {
                id: mrs

                focus: true
                clip:  true

                model: gpm.mrModel
                selectionModel: ItemSelectionModel { }

                columnWidthProvider: function(column) {
                   let ew = explicitColumnWidth(column);
                   let iw = implicitColumnWidth(column);
                   let hw = header.implicitColumnWidth(column);

                   console.log("ew=", ew, "  iw=", iw, "  hw=", hw, "  max=", max(hw,ew, iw));

                   if (ew >= 0)
                       return w;

                   return max(hw, iw, ew)
                }

                delegate: Item {
                    implicitWidth: itemText.implicitWidth + urlButton.implicitWidth
                    implicitHeight: itemText.implicitHeight

                    Rectangle {
                        anchors.fill: parent
                        color: row == mrs.currentRow ? palette.highlight : ((row % 2) == 0 ? palette.alternateBase : palette.base)

                        Text {
                            id: urlButton
                            visible: column == 0
                            padding: 5
                            text: "🌐"

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: Qt.openUrlExternally(model.url)
                            }
                        }

                        Text {
                            id: itemText

                            anchors.fill: parent
                            anchors.leftMargin: urlButton.visible ? urlButton.width - 5 : 0
                            padding: 5
                            text: model.display
                            font.bold: model.font.bold
                            color: mrs.currentRow ? palette.highlightedText : palette.text
                            ToolTip.delay: 500
                            ToolTip.timeout: 3000
                            ToolTip.text: model.toolTip
                            ToolTip.visible: model.toolTip ? itemMa.containsMouse : false;

                            MouseArea {
                                id: itemMa
                                visible: model.toolTip
                                anchors.fill: parent
                                hoverEnabled: true
                            }
                        }
                    }
                }
            }
        }
    }
}
