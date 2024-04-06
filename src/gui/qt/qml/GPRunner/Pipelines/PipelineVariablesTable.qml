import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.qmlmodels
import mudbay.gprunner.models

import GPRunner

Item {
    required property var currentProject

    clip: true

    function getVariables() {
        return variableModel.variables()
    }

    function setVariables(variables) {
        variableModel.setVariables(variables)
    }

    VariableModel {
        id: variableModel
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        HorizontalHeaderView {
            id: header
            syncView: variables
            Layout.fillWidth: true
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TableView {
                id: variables

                anchors.fill: parent

                focus: true
                clip: true

                model: variableModel
                selectionModel: ItemSelectionModel { }

                columnWidthProvider: Utility.calcColumnWidth.bind(this, header)

                delegate: Rectangle {
                    required property bool current

                    implicitWidth: textItem.implicitWidth
                    implicitHeight: textItem.implicitHeight

                    color: row == variables.currentRow ? palette.highlight : ((row % 2) == 0 ? palette.alternateBase : palette.base)
                    border.width: current ? 1 : 0

                    TableView.editDelegate: DelegateChooser {
                        DelegateChoice { column: VariableModel.Key; TextField { text: model.edit; TableView.onCommit: model.edit = text } }
                        DelegateChoice { column: VariableModel.Value; TextField { text: model.edit; TableView.onCommit: model.edit = text } }
                        DelegateChoice { column: VariableModel.Used; CheckBox  { checked: model.edit; onToggled: model.edit = checked }}
                    }

                    Label {
                        id: textItem

                        anchors.fill: parent
                        padding: 5
                        text: model.display
                        color: variables.currentRow ? palette.highlightedText : palette.text

                        wrapMode: Text.WordWrap
                    }
                }
            }
        }

        RowLayout {
            Button {
                enabled: currentProject != null
                text: "+"
                onClicked: variableModel.addVariable("variable", "value", false)
            }

            Button {
                enabled: currentProject != null && variables.currentRow >= 0
                text: "-"
                onClicked: variableModel.removeRow(variables.currentRow)
            }
        }
    }
}
