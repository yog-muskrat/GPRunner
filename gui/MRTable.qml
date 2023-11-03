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
                    required property bool current
                    required property bool selected

                    implicitWidth: itemText.implicitWidth
                    implicitHeight: itemText.implicitHeight

                    Rectangle {
                        anchors.fill: parent
                        border.width: current ? 1 : 0
                        color: selected ? "#DEDEFE" : ((row % 2) == 0 ? "#EFEFEF" : "transparent")

                        RoundButton {
                            text: "🌐"
                            flat: true
                            visible: column == 0
                            onClicked: Qt.openUrlExternally(model.url)
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
