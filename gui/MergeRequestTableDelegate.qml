import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import mudbay.gprunner.models

Rectangle {
    signal mrSelected(mr: var)

    id: mrDelegate

    implicitWidth: delegateLayout.implicitWidth
    implicitHeight: delegateLayout.implicitHeight

    color: row == mrs.currentRow ? palette.highlight : palette.base

    function isApproved(mr, column) {
        if(column == MRModel.Assignee) return mr.isApprovedBy(mr.assignee)
        if(column == MRModel.Reviewer) return mr.isApprovedBy(mr.reviewer)
        return false;
    }

    function canApprove(mr, column) {
        return (column == MRModel.Assignee && mr.assignee == gpm.currentUser)
        || (column == MRModel.Reviewer && mr.reviewer == gpm.currentUser);
    }

    function avatarUrl(mr, column) {
        if(column == MRModel.Author)   return mr.author.avatarUrl
        if(column == MRModel.Assignee) return mr.assignee.avatarUrl
        if(column == MRModel.Reviewer) return mr.reviewer.avatarUrl
        return ""
    }

    function isUserInvolved(mr, column) {
        return mr.isUserInvolved(gpm.currentUser)
    }

    function getUser(column) {
        if(column == MRModel.Author)   return mr.author
        if(column == MRModel.Assignee) return mr.assignee
        if(column == MRModel.Reviewer) return mr.reviewer
        return false
    }

    function isUserColumn(column) { return column == MRModel.Author || column == MRModel.Assignee || column == MRModel.Reviewer }

    RowLayout {
        id: delegateLayout

        anchors.fill: parent
        spacing: 0

        TextLinkButton {
            leftPadding: 5
            visible: column == MRModel.Iid
            url: mr.url
        }

        TextLinkButton {
            leftPadding: 5
            text: model.display
            visible: column == MRModel.Pipeline
            url: mr.pipeline ? mr.pipeline.url : ""
            toolTip: model.edit
        }

        Image {
            visible: isUserColumn(column)
            Layout.maximumHeight: 28
            Layout.minimumHeight: 28
            Layout.maximumWidth: Layout.maximumHeight
            Layout.minimumWidth: Layout.maximumWidth
            source: avatarUrl(mr, column)
            fillMode: Image.PreserveAspectFit
        }

        Label {
            padding: 5

            visible: column != MRModel.Pipeline
            text: model.display
            font.bold: isUserInvolved(mr, column)
            color: mrs.currentRow ? palette.highlightedText : palette.text

            DefaultToolTip {
                toolTipText: model.toolTip
            }
            TapHandler {
                onTapped: {
                    mrDelegate.mrSelected(model.mr)
                }
            }
        }

        Label {
            id: approveCheckbox

            visible: {
                if(column != MRModel.Assignee && column != MRModel.Reviewer) return false
                return isApproved(mr, column) || canApprove(mr, column)
            }

            rightPadding: 5

            text: isApproved(mr, column) ? "☑" : canApprove(mr, column) ? "☐" : ""
            color: canApprove(mr, column) ? (isApproved(mr, column) ? "green" : "yellow") : palette.text

            DefaultToolTip {
                toolTipText: canApprove(mr, column) ? (isApproved(mr, column) ? "Unapprove" : "Approve") : ""
            }

            HoverHandler {
                enabled: canApprove(mr, column)
                cursorShape: Qt.PointingHandCursor
            }

            TapHandler {
                enabled: canApprove(mr, column)
                onTapped: isApproved(mr, column) ? mr.unapprove() : mr.approve()
            }
        }

        UnreadMarker {
            mr: model.mr
            visible: column == MRModel.Discussions && model.mr.hasUnreadNotes
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

        currentIndex: currentIndex < 0 ? indexOfValue(getUser(column)) : currentIndex

        implicitContentWidthPolicy: ComboBox.WidestText
        TableView.onCommit: edit = currentValue

        Pane{
            visible: getUser(column).id > 0
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
