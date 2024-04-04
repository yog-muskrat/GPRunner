import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.qmlmodels
import mudbay.gprunner.models
import "Utility.js" as Utility

DelegateChooser {
    DelegateChoice {
        column: MRModel.Title

        MrDelegate {
            ColumnLayout {
                RowLayout {
                    UnreadMarker {
                        mr: model.mr
                        visible: model.mr.hasUnreadNotes
                    }
                    Label {
                        text: display
                        font.bold: mr.isUserInvolved(gpm.currentUser)
                    }
                }
                RowLayout {
                    TextLinkButton {
                        url: mr.url
                        text: "!" + mr.iid
                        font.pointSize: 9
                    }
                    Label {
                        text: "Created at " + Utility.formatDateTime(mr.createdAt) + " by "
                        font.pointSize: 9
                    }
                    User {
                        user: model.mr.author
                        size: 19
                        showLabel: false
                    }
                }
            }
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
                toolTip: "Pipeline: " + edit
            }
        }
    }

	DelegateChoice {
        column: MRModel.Discussions

        MrDelegate {
            ColumnLayout {
                Label { text: display }
                Label {
                    font.pointSize: 9
                    text: "Updated at " + Utility.formatDateTime(mr.updatedAt)
                }
            }
        }
    }

	DelegateChoice {
        column: MRModel.Users

        MrDelegate {
            MrUser {
                user: model.mr.assignee
                editable: model.mr.author == gpm.currentUser
                mr: model.mr
                showLabel: false
                size: 28
                font.bold: mr.isUserInvolved(gpm.currentUser)
                onUserSelected: (newUser) => { mr.assignee = newUser }
                onUserCleared: {
                    mr.clearAssignee()
                    console.log("Clear assignee!")
                }
            }
            MrUser {
                user: model.mr.reviewer
                editable: model.mr.author == gpm.currentUser
                mr: model.mr
                showLabel: false
                size: 28
                font.bold: mr.isUserInvolved(gpm.currentUser)
                onUserSelected: (newUser) => { mr.reviewer = newUser }
                onUserCleared: {
                    mr.clearReviewer()
                    console.log("Clear reviewer!")
                }
            }
        }
    }

	DelegateChoice {
        column: MRModel.Branches

        MrDelegate {
            Label {
                text: mr.sourceBranch + "\n" + mr.targetBranch
            }
        }
    }
}
