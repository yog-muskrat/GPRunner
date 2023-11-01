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

                model:    gpm.mrModel
                delegate: Item {
                    implicitWidth: itemText.implicitWidth
                    implicitHeight: itemText.implicitHeight

                    Rectangle {
                        anchors.fill: parent
                        color: (row % 2) == 0 ? "#EFEFEF" : "transparent"
                    }

                    Text {
                        id: itemText

                        anchors.fill: parent
                        padding: 5
                        text: model.display
                        font: model.font
                    }
                }
            }
        }
    }
}
