import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "Utility.js" as Utility

Item {
    required property int currentProject

    property int currentMrId: 0
    property int currentMrIid: 0

    clip: true

    onCurrentProjectChanged: {
        currentMrId = 0
        currentMrIid = 0
    }

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
                selectionModel: ItemSelectionModel {}

                columnWidthProvider: Utility.calcColumnWidth.bind(this, header)

                delegate: Rectangle {
                    implicitWidth: delegateLayout.implicitWidth
                    implicitHeight: delegateLayout.implicitHeight

                    color: row == mrs.currentRow ? palette.highlight : palette.base

                    RowLayout {
                        id: delegateLayout

                        anchors.fill: parent
                        spacing: 0

                        TextLinkButton {
                            leftPadding: 5
                            visible: model.url ? true : false
                            url: model.url
                        }

                        Image {
                            visible: model.user ? true : false
                            Layout.maximumHeight: 28
                            Layout.maximumWidth: Layout.maximumHeight
                            source: model.user ? model.user.avatarUrl : ""
                            fillMode: Image.PreserveAspectFit
                        }

                        Label {
                            padding: 5

                            text: model.display
                            font.bold: model.user ? (model.user.username == gpm.currentUser.username) : false
                            color: mrs.currentRow ? palette.highlightedText : palette.text

                            DefaultToolTip { toolTipText: model.toolTip }
                            TapHandler {
                                onTapped: {
                                    currentMrId = model.id
                                    currentMrIid = model.iid
                                }
                            }
                        }

                        Label {
                            visible: model.canApprove || model.canUnapprove || model.isApproved

                            rightPadding: 5

                            text: isApproved ? "☑" : canApprove ? "☐" : ""
                            color: canApprove ? "yellow" : canUnapprove ? "green" : palette.text

                            DefaultToolTip { toolTipText: canApprove ? "Approve" : canUnapprove ? "Unapprove" : "" }

                            HoverHandler {
                                enabled: model.canApprove || model.canUnapprove
                                cursorShape: Qt.PointingHandCursor
                            }

                            TapHandler {
                                enabled: model.canApprove || model.canUnapprove
                                onTapped: model.canApprove ? gpm.approveMR(currentProject, model.iid) : gpm.unapproveMR(currentProject, model.iid)
                            }
                        }

                        UnreadMarker {
                            projectId: currentProject
                            mrIid: model.iid

                            visible: model.hasUnreadNotes
                            rightPadding: 5
                        }

                        HorizontalSpacer { }
                    }
                }
            }
        }
    }
}
