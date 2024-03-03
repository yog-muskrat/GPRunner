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
                Label {
                    text: display
                    font.bold: mr.isUserInvolved(gpm.currentUser)
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
                toolTip: edit
            }
        }
    }

	DelegateChoice {
        column: MRModel.Discussions

        MrDelegate {
            Label { text: display }
            UnreadMarker {
                mr: model.mr
                visible: model.mr.hasUnreadNotes
            }
        }
    }

	DelegateChoice {
        column: MRModel.Users

        MrDelegate {
            ColumnLayout {
                MrUser {
                    user: model.mr.assignee
                    mr: model.mr
                    showLabel: false
                    size: 19
                    font.bold: mr.isUserInvolved(gpm.currentUser)
                }
                MrUser {
                    user: model.mr.reviewer
                    mr: model.mr
                    showLabel: false
                    size: 19
                    font.bold: mr.isUserInvolved(gpm.currentUser)
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

	DelegateChoice {
        column: MRModel.CreatedUpdated

        MrDelegate {
            Label {
                text: Utility.formatDateTime(mr.updatedAt)
            }
        }
    }
}

/*Rectangle {
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
