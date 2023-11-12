import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "Utility.js" as Utility

Item {
    required property int currentProject

    clip: true

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

                model: gpm.variableModel
                selectionModel: ItemSelectionModel {
                }

                columnWidthProvider: Utility.calcColumnWidth.bind(this, header)

                delegate: Rectangle {
                    required property bool current

                    implicitWidth: textItem.implicitWidth
                    implicitHeight: textItem.implicitHeight

                    color: row == variables.currentRow ? palette.highlight : ((row % 2) == 0 ? palette.alternateBase : palette.base)
                    border.width: current ? 1 : 0

                    TableView.editDelegate: {
                        return (typeof (model.edit) == "boolean") ? checkDelegate : textDelegate;
                    }

                    Label {
                        id: textItem

                        anchors.fill: parent
                        padding: 5
                        text: model.display
                        color: variables.currentRow ? palette.highlightedText : palette.text

                        wrapMode: Text.WordWrap
                    }

                    Component {
                        id: textDelegate

                        TextField {
                            anchors.fill: parent
                            text: model.edit
                            TableView.onCommit: model.edit = text
                            Component.onCompleted: selectAll()
                            wrapMode: TextInput.WordWrap
                            font.pointSize: 9
                            padding: 5
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
                }
            }
        }

        RowLayout {
            Button {
                enabled: currentProject > 0
                text: "+"
                onClicked: gpm.addVariable()
            }

            Button {
                enabled: currentProject > 0 && variables.currentRow >= 0
                text: "-"
                onClicked: gpm.removeVariable(variables.currentRow)
            }
        }
    }
}
