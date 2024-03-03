import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.qmlmodels
import mudbay.gprunner.models
import "Utility.js" as Utility

DelegateChooser {
    DelegateChoice {
        column: MRModel.Iid

        MrDelegate {
            TextLinkButton { url: mr.url }
            Label { text: display }
            HorizontalSpacer {}
        }
    } 

	DelegateChoice {
        column: MRModel.Status

        MrDelegate {
            Label {
                text: display

                DefaultToolTip { toolTipText: toolTip }
            }
        }
    }

	DelegateChoice {
        column: MRModel.Pipeline

        MrDelegate {
            TextLinkButton {
                text: display
                url: mr.pipeline ? mr.pipeline.url : ""
                toolTip: edit
            }
        }
    }

	DelegateChoice {
        column: MRModel.Discussions

        MrDelegate {
            Label {
                text: display
            }

            UnreadMarker {
                mr: model.mr
                visible: model.mr.hasUnreadNotes
            }
        }
    }

	DelegateChoice {
        column: MRModel.Title

        MrDelegate {
            Label {
                text: display
                font.bold: mr.isUserInvolved(gpm.currentUser)
            }
        }
    }

	DelegateChoice {
        column: MRModel.Author

        MrDelegate {
            User {
                user: model.mr.author
                boldFont: mr.isUserInvolved(gpm.currentUser)
            }
        }
    }

	DelegateChoice {
        column: MRModel.Assignee

        MrDelegate {
            User {
                user: model.mr.assignee
                boldFont: mr.isUserInvolved(gpm.currentUser)
            }
        }
    }

	DelegateChoice {
        column: MRModel.Reviewer

        MrDelegate {
            User {
                user: model.mr.reviewer
                boldFont: mr.isUserInvolved(gpm.currentUser)
            }
        }
    }

	DelegateChoice {
        column: MRModel.Branches

        MrDelegate {
            Label {
                text: "From: " + mr.sourceBranch + "\nTo:" + mr.targetBranch
            }
        }
    }

	DelegateChoice {
        column: MRModel.CreatedUpdated

        MrDelegate {
            Label {
                text: "Created: " + Utility.formatDateTime(mr.createdAt) + "\nUpdated: " + Utility.formatDateTime(mr.updatedAt)
            }
        }
    }
}

/*Rectangle {
    function isApproved(mr, column) {
        if(column == MRModel.Assignee) return mr.isApprovedBy(mr.assignee)
        if(column == MRModel.Reviewer) return mr.isApprovedBy(mr.reviewer)
        return false;
    }

    function canApprove(mr, column) {
        return (column == MRModel.Assignee && mr.assignee == gpm.currentUser)
        || (column == MRModel.Reviewer && mr.reviewer == gpm.currentUser);
    }

    RowLayout {
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
*/