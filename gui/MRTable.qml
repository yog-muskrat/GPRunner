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

                    color: row == mrs.currentRow ? palette.highlight : palette.base

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

                            leftPadding: 5
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
                            onTapped: model.canApprove ? gpm.approveMR(currentProject, model.iid) : gpm.unapproveMR(currentProject, model.iid)
                        }

                        Label {
                            id: approveButtonLabel

                            leftPadding: 5
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


                    Item {
                        id: unreadIndicator

                        anchors.right: parent.right
                        anchors.verticalCenter: itemText.verticalCenter

                        visible: model.hasUnreadNotes
                        implicitWidth: visible ? unreadIndicatorLabel.implicitWidth : 0
                        implicitHeight: visible ? unreadIndicatorLabel.implicitHeight : 0

                        Label {
                            id: unreadIndicatorLabel

                            rightPadding: 5
                            text: "●"
                            color: "#B21818"
                        }
                    }
                }
            }
        }
    }
}
