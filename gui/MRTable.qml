import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    border {width: 1; color: "#424242"}

    ColumnLayout {
        anchors.fill:    parent
        anchors.margins: 1
      
        spacing: 0

        CommonHeaderView {
            syncView: mrs
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth:  true

            TableView {
                id: mrs

                focus: true
                clip:  true

                model: gpm.mrModel
                selectionModel: ItemSelectionModel { }

                delegate: Item {
                    implicitWidth: itemText.implicitWidth
                    implicitHeight: itemText.implicitHeight

                    Rectangle {
                        anchors.fill: parent
                        color: row == mrs.currentRow ? "#DEDEFE" : ((row % 2) == 0 ? "#EFEFEF" : "transparent")

                        MouseArea {
                            cursorShape: Qt.PointingHandCursor
                            width:  itemText.implicitHeight
                            height: itemText.implicitHeight
                            visible: column == 0
                            onClicked: Qt.openUrlExternally(model.url)

                            Text {
                                anchors.centerIn: parent
                                text: "🌐"
                                font: model.font
                            }
                        }

                        Text {
                            id: itemText

                            anchors.fill: parent
                            topPadding: 5
                            bottomPadding: 5
                            rightPadding: 5
                            leftPadding: column == 0 ? 30 : 5
                            text: model.display
                            font: model.font
                        }
                    }
                }
            }
        }
    }
}
