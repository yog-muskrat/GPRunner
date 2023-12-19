import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    required property int currentProject
    required property int currentMrId
    required property int currentMrIid

    id: discussions

    onCurrentMrIdChanged: gpm.setCurrentMR(currentProject, currentMrId)

    function expandCollapseIf(pred, expand) {
        for(let row = 0; row < mrs.rows; row++) {
            let idx = mrs.index(row, 0)
            if(idx.parent.valid) continue;
            if(pred(idx)) expand ? mrs.expand(row) : mrs.collapse(row)
        }
    }

    // TODO: Получать роль по имени
    function isResolved(idx)   { return mrs.model.data(idx, 261) == true }
    function isUnresolved(idx) { return mrs.model.data(idx, 261) != true }

    ColumnLayout
    {
        anchors.fill: parent

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TreeView {
                id: mrs
                anchors.fill: parent

                clip: true

                model: gpm.discussionModel
                columnWidthProvider: function (column) { return width }

                delegate: DiscussionDelegate {
                    projectId: discussions.currentProject
                    mrIid: discussions.currentMrIid

                    onAddNoteRequested: function(discussionId) {
                        dialog.discussionId = discussionId
                        dialog.open()
                    }

                    onEditNoteRequested: function(discussionId, noteId, noteText) {
                        dialog.discussionId = discussionId
                        dialog.noteId = noteId
                        dialog.noteText = noteText
                        dialog.open()
                    }

                    onRemoveNoteRequested: function(discussionId, noteId) { gpm.removeMRDiscussionNote(currentProject, currentMrIid, discussionId, noteId) }

                    onResolveRequested: function(discussionId) { gpm.resolveMRDiscussion(currentProject, currentMrIid, discussionId) }

                    onUnresolveRequested: function(discussionId) { gpm.unresolveMRDiscussion(currentProject, currentMrIid, discussionId) }
                }

                DiscussionNoteDialog {
                    id: dialog

                    currentProject: discussions.currentProject
                    currentMrIid: discussions.currentMrIid

                    width: Math.max(parent.width / 2, 500)
                    height: Math.max(parent.height / 2, 350)

                    anchors.centerIn: parent
                }
            }
        }

        RowLayout {
                visible: currentMrIid > 0

            Button {
                text: "🗨"
                onClicked: { dialog.open() }
                DefaultToolTip {
                    toolTipText: "Add discussion"
                }
            }

            HorizontalSpacer {}

            Button {
                visible: mrs.rows > 0
                text: "Expand/collapse all"
                onClicked: {
                    expandCollapseIf((x)=>true, !mrs.isExpanded(0))
                }
            }

            Button {
                visible: mrs.rows > 0
                text: "Collapse resolved"
                onClicked: { expandCollapseIf(isResolved, false) }
            }

            Button {
                visible: mrs.rows > 0
                text: "Expand unresolved"
                onClicked: { expandCollapseIf(isUnresolved, true) }
            }
        }
    }
}
