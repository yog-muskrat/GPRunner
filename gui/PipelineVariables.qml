import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQml.Models

Rectangle {
    border {width: 1; color: "#424242"}

    ColumnLayout {
        anchors.fill:    parent
        anchors.margins: 1

        spacing: 0

        CommonHeaderView {
            syncView: variables
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
                    id: textItem
                    text: model.display
                    anchors.centerIn: parent
                }

                Component {
                    id: textDelegate

                    TextField {
                        anchors.fill: parent
                        horizontalAlignment: TextInput.AlignHCenter
                        verticalAlignment:   TextInput.AlignVCenter
                        text: model.edit
                        TableView.onCommit: model.edit = text
                        Component.onCompleted: selectAll()
                    }
                }

                Component {
                    id: checkDelegate

                    CheckBox {
                        anchors.centerIn: parent
                        checked: model.edit
                        onToggled: model.edit = checked
                    }
                }

                TableView.editDelegate: { return (typeof(model.edit) == "boolean") ? checkDelegate : textDelegate }
            }
        }

        RowLayout {
            spacing: 0

            Button {
                Layout.fillWidth: true
                Layout.horizontalStretchFactor: 1
                text: "Добавить"
                onClicked: gpm.addVariable()
            }

            Button {
                Layout.fillWidth: true
                Layout.horizontalStretchFactor: 1
                enabled: variables.currentRow > 0
                text: "Удалить"
                onClicked: gpm.removeVariable(variables.currentRow)
            }
        }
    }
}
