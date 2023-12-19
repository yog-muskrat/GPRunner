import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    required property int currentProject
    property int mrIid: 0
    signal mrSelected(id: int, iid: int)

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
            visible: model.url ? true : false
            url: model.url
        }

        TextLinkButton {
            leftPadding: 5
            text: model.display
            visible: model.pipelineUrl ? true : false
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
                    mrIid = model.iid
                    mrDelegate.mrSelected(model.id, model.iid)
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
                onTapped: model.canApprove ? gpm.approveMR(currentProject, model.iid) : gpm.unapproveMR(currentProject, model.iid)
            }
        }

        UnreadMarker {
            projectId: currentProject
            mrIid: model.iid

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
