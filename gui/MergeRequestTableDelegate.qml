import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import mudbay.gprunner.models

Rectangle {
    required property int currentProject
    signal mrSelected(mr: var)

    id: mrDelegate

    implicitWidth: delegateLayout.implicitWidth
    implicitHeight: delegateLayout.implicitHeight

    color: row == mrs.currentRow ? palette.highlight : palette.base

    RowLayout {
        id: delegateLayout

        anchors.fill: parent
        spacing: 0

        TextLinkButton {
            leftPadding: 5
            visible: column == MRModel.Iid
            url: model.url
        }

        TextLinkButton {
            leftPadding: 5
            text: model.display
            visible: column == MRModel.Pipeline
            url: model.pipelineUrl
            toolTip: model.edit
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

            visible: model.pipelineUrl ? false : true
            text: model.display
            font.bold: model.user ? (model.user.username == gpm.currentUser.username) : false
            color: mrs.currentRow ? palette.highlightedText : palette.text

            DefaultToolTip {
                toolTipText: model.toolTip
            }
            TapHandler {
                onTapped: {
                    console.log("MR:", model.mr)
                    mrDelegate.mrSelected(model.mr)
                }
            }
        }

        Label {
            visible: model.canApprove || model.canUnapprove || model.isApproved

            rightPadding: 5

            text: isApproved ? "☑" : canApprove ? "☐" : ""
            color: canApprove ? "yellow" : canUnapprove ? "green" : palette.text

            DefaultToolTip {
                toolTipText: canApprove ? "Approve" : canUnapprove ? "Unapprove" : ""
            }

            HoverHandler {
                enabled: model.canApprove || model.canUnapprove
                cursorShape: Qt.PointingHandCursor
            }

            TapHandler {
                enabled: model.canApprove || model.canUnapprove
                onTapped: model.canApprove ? gpm.approveMR(currentProject, model.mr.iid) : gpm.unapproveMR(currentProject, model.mr.iid)
            }
        }

        UnreadMarker {
            projectId: currentProject
            mr: model.mr

            visible: model.hasUnreadNotes
            rightPadding: 5
        }

        HorizontalSpacer {
        }
    }

    TableView.editDelegate: ComboBox {
        id: combo

        background: Pane{}
        model: gpm.activeUsers
        textRole: "username"

        currentIndex: indexOfValue(user)

        implicitContentWidthPolicy: ComboBox.WidestText
        TableView.onCommit: edit = currentValue.id

        Pane{
            visible: user.id > 0
            padding: 0

            anchors.left: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            Button {
                anchors.fill: parent

                text: "X"

                onClicked: {
                    edit = 0
                    mrDelegate.TableView.view.closeEditor()
                }
            }
        }
    }
}
