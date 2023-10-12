import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQml.Models

Rectangle {
    Layout.fillHeight: true
    Layout.fillWidth:  true

    border {width: 1; color: "#424242"}

    ColumnLayout {
        anchors.fill:    parent
        anchors.margins: 1

        HorizontalHeaderView {
            id: variablesHeader
            syncView: variables
            clip: true
            delegate: Rectangle {
                implicitWidth:  itemText.implicitWidth
                implicitHeight: itemText.implicitHeight

                border {width: 1; color: "#424242"}
                color: "#EFEFEF"

                Text {
                    id: itemText

                    anchors.centerIn: parent
                    padding: 5
                    text: model.display
                }
            }
        }

        TableView {
            id: variables

            Layout.fillHeight: true
            Layout.fillWidth:  true

            focus: true
            clip: true

            model:             gpm.variableModel
            selectionModel:    ItemSelectionModel { }
            selectionBehavior: TableView.SelectRows
            selectionMode:     TableView.SingleSelection

            delegate: Rectangle {
                required property bool current

                implicitWidth:  TableView.view.width / 3
                implicitHeight: 25

                color: current ? "#AEAEFE" : "#FEFEFE"
                border { width: 1; color: current ? "#8484FE" : "#AEAEAE" }

                Text {
                    text: model.display
                    anchors.centerIn: parent
                }

                TableView.editDelegate: Item {
                    anchors.fill: parent

                    TextField {
                        visible:      column != 2
                        anchors.fill: parent
                        text:         model.edit
                        onAccepted:   model.edit = text
                        Component.onCompleted: selectAll()
                    }
                
                    CheckBox {
                        visible:          column == 2
                        anchors.centerIn: parent
                        checked:          model.edit
                        onToggled:        model.edit = checked
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true

            Button {
                Layout.fillWidth: true
                text: "Добавить"
                onClicked: gpm.addVariable()
            }

            Button {
                Layout.fillWidth: true
                enabled: variables.currentRow > 0
                text: "Удалить"
                onClicked: gpm.removeVariable(variables.currentRow)
            }
        }
    }
}