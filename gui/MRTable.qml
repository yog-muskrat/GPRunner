import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "Utility.js" as Utility

Item {
    required property int currentProject

    property int currentMrId: 0
    property int currentMrIid: 0

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
                    implicitWidth: urlButton.implicitWidth + approveButton.implicitWidth + itemText.implicitWidth + unreadIndicator.implicitWidth
                    implicitHeight: itemText.implicitHeight

                    color: row == mrs.currentRow ? palette.highlight : ((row % 2) == 0 ? palette.alternateBase : palette.base)

                    Item {
                        id: urlButton
                        
                        anchors.left: parent.left
                        anchors.verticalCenter: itemText.verticalCenter
                        
                        visible: model.url
                        implicitWidth: visible ? urlButtonLabel.implicitWidth : 0
                        implicitHeight: visible ? urlButtonLabel.implicitHeight: 0

                        HoverHandler {
                                id: urlHover
                                cursorShape: Qt.PointingHandCursor
                        }
                        TapHandler { onTapped: Qt.openUrlExternally(model.url) }

                        Label {
                            id: urlButtonLabel
                            text: "🌐"

                            ToolTip.delay: 500
                            ToolTip.timeout: 3000
                            ToolTip.text: "Open MR in browser"
                            ToolTip.visible: urlHover.hovered
                        }
                    }

                    Item {
                        id: approveButton

                        anchors.left: urlButton.right
                        anchors.verticalCenter: itemText.verticalCenter

                        visible: model.canApprove || model.canUnapprove || model.isApproved
                        implicitWidth: visible ? approveButtonLabel.implicitWidth : 0
                        implicitHeight: visible ? approveButtonLabel.implicitHeight: 0

                        HoverHandler {
                            id: approveHover
                            enabled: model.canApprove || model.canUnapprove
                            cursorShape: Qt.PointingHandCursor }

                        TapHandler {
                            enabled: model.canApprove || model.canUnapprove
                            onTapped: model.canApprove ? gpr.approveMR(currentProject, currentMrId) : gpr.unapproveMR(currentProject, currentMrId)
                        }

                        Label {
                            id: approveButtonLabel

                            text: isApproved ? "☑" : canApprove ? "☐" : ""
                            color: canApprove ? "yellow" : canUnapprove ? "green" : palette.text
                            ToolTip.delay: 500
                            ToolTip.timeout: 3000
                            ToolTip.text: canApprove ? "Approve" : canUnapprove ? "Unapprove" : ""
                            ToolTip.visible: approveHover.hovered
                        }
                    }

                    Label {
                        id: itemText

                        anchors.left: approveButton.right
                        anchors.right: unreadIndicator.left

                        padding: 5
                        text: model.display
                        font.bold: model.font.bold
                        color: mrs.currentRow ? palette.highlightedText : palette.text
                        ToolTip.delay: 500
                        ToolTip.timeout: 3000
                        ToolTip.text: model.toolTip
                        ToolTip.visible: model.toolTip ? hoverHandler.hovered : false

                        HoverHandler { id: hoverHandler }
                        TapHandler {
                            onTapped: {
                                currentMrId = model.id
                                currentMrIid = model.iid
                            }
                        }
                    }


                    Rectangle {
                        id: unreadIndicator

                        visible: model.hasUnreadNotes

                        anchors.right: parent.right
                        anchors.rightMargin: 5
                        anchors.verticalCenter: itemText.verticalCenter

                        width: visible ? itemText.implicitHeight / 3 : 0
                        height: width
                        radius: width / 2
                        color: "#B21818"
                    }
                }
            }
        }
    }
}
