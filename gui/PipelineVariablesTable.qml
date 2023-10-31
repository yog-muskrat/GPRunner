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

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth:  true

            TableView {
                id: variables

                focus: true
                clip: true

                model:             gpm.variableModel
                selectionModel:    ItemSelectionModel { }

                delegate: Rectangle {
                    required property bool current

                    implicitWidth:  textItem.implicitWidth
                    implicitHeight: textItem.implicitHeight

                    color: current ? "#AEAEFE" : "#FEFEFE"
                    border { width: 1; color: current ? "#8484FE" : "#AEAEAE" }

                    Text {
                        id: textItem

                        anchors.fill: parent
                        padding: 5
                        text: model.display

                        wrapMode: Text.WordWrap
                    }

                    Component {
                        id: textDelegate

                        TextField {
                            horizontalAlignment: TextInput.AlignHCenter
                            verticalAlignment:   TextInput.AlignVCenter
                            text: model.edit
                            TableView.onCommit: model.edit = text
                            Component.onCompleted: selectAll()
                            wrapMode: TextInput.WordWrap
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
        }

        RowLayout {
            spacing: 0

            Button {
                //Layout.fillWidth: true
                //Layout.horizontalStretchFactor: 1
                text: "Добавить"
                onClicked: gpm.addVariable()
            }

            Button {
                //Layout.fillWidth: true
                //Layout.horizontalStretchFactor: 1
                enabled: variables.currentRow > 0
                text: "Удалить"
                onClicked: gpm.removeVariable(variables.currentRow)
            }
        }
    }
}
