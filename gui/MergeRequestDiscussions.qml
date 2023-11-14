import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ScrollView {
    required property int currentProject
    required property int currentMrId
    required property int currentMrIid

    id: scrollView

    onCurrentMrIdChanged: gpm.setCurrentMR(currentProject, currentMrId)

    TreeView {
        id: mrs
        anchors.fill: parent

        clip: true

        model: gpm.discussionModel
        columnWidthProvider: function (column) {
            return width;
        }

        //delegate: TreeViewDelegate{}
        delegate: Item {
            readonly property real indent: 20
            readonly property real padding: 5
            readonly property real imgSize: 48

            readonly property bool isNote: depth == 1
            readonly property bool isDiscussion: depth == 0

            // Assigned to by TreeView:
            required property TreeView treeView
            required property bool isTreeNode
            required property bool expanded
            required property int hasChildren
            required property int depth

            id: treeDelegate

            implicitWidth:  isNote ? note.implicitWidth : discussion.implicitWidth
            implicitHeight: isNote ? note.implicitHeight : discussion.implicitHeight

            Item {
                id: discussion
                visible: isDiscussion

                implicitWidth: discussionHeader.implicitWidth + discussionNotes.implicitWidth + unreadIndicator.implicitWidth + discussionStatus.implicitWidth
                implicitHeight: Math.max(discussionHeader.implicitHeight, discussionStatus.implicitHeight)
                width: treeDelegate.width

                Rectangle {
                    anchors.fill: parent
                    color: discussionHeaderHover.hovered ? palette.base : palette.alternateBase
                }

                Item { 
                    id: discussionHeader

                    implicitWidth: indicator.implicitWidth + discussionTitle.implicitWidth
                    implicitHeight: Math.max(indicator.implicitHeight, discussionTitle.implicitHeight)

                    TapHandler {
                        onTapped: treeView.toggleExpanded(row)
                    }

                    HoverHandler {
                        id: discussionHeaderHover
                        cursorShape: Qt.PointingHandCursor
                    }

                    Label {
                        id: indicator

                        text: ">"
                        rotation: treeDelegate.expanded ? 90 : 0
                        padding: treeDelegate.padding
                    }

                    Label {
                        id: discussionTitle

                        anchors.left: indicator.right
                        anchors.verticalCenter: indicator.verticalCenter

                        text: model.display
                        padding: treeDelegate.padding
                    }
                }

                Item {
                    id: discussionNotes

                    visible: model.noteCount > 0
                    implicitWidth: visible ? noteCountLabel.implicitWidth : 0
                    implicitHeight: visible ? noteCountLabel.implicitHeight : 0

                    anchors.right: unreadIndicator.left
                    anchors.verticalCenter: discussionHeader.verticalCenter

                    Label {
                        id: noteCountLabel

                        visible: model.noteCount > 0

                        rightPadding: 5
                        text: "[" + model.noteCount + "]"
                    }
                }

                Item {
                    id: unreadIndicator

                    anchors.right: discussionStatus.left
                    anchors.verticalCenter: discussionHeader.verticalCenter

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

                Label {
                    id: discussionStatus

                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: treeDelegate.padding * 2

                    visible: model.resolvable

                    text: model.canResolve ? "[Resolve] ☐" : model.canUnresolve ? "[Unresolve] ☑" : model.resolved ? "☑" : "☐"
                    color: model.canResolve ? "yellow" : model.canUnresolve ? "green" : palette.text

                    HoverHandler {
                        enabled: model.canResolve || model.canUnresolve
                        cursorShape: Qt.PointingHandCursor
                    }

                    TapHandler {
                        enabled: model.canResolve || model.canUnresolve
                        onTapped: {
                            if(model.canResolve) gpm.resolveMRDiscussion(currentProject, currentMrIid, model.discussionId)
                            else if(model.canUnresolve) gpm.unresolveMRDiscussion(currentProject, currentMrIid, model.discussionId)
                        }
                    }
                }
            }

            Item {
                id: note
                visible: treeDelegate.isNote

                implicitHeight: noteHeader.implicitHeight + noteBody.implicitHeight
                implicitWidth: Math.max(noteHeader.implicitWidth, noteBody.implicitWidth)
                width: treeDelegate.width

                Rectangle {
                    anchors.fill: parent
                    color: palette.base
                }

                Item {
                    id: noteHeader

                    x: indent * depth
                    y: padding
                    implicitHeight: image.height + padding
                    implicitWidth: image.width + authorLabel.implicitWidth

                    Image {
                        id: image

                        width:  imgSize
                        height: imgSize
                        source: model.avatar
                        fillMode: Image.PreserveAspectFit
                    }

                    Label {
                        id: authorLabel

                        anchors.left: image.right
                        anchors.top: image.top
                        text: model.author + "\n" + new Date(model.created).toLocaleString(Qt.locale("ru_RU"), Locale.ShortFormat)
                        leftPadding: treeDelegate.padding
                    }
                }

                Label {
                    id: noteBody

                    anchors.top: noteHeader.bottom
                    anchors.left: noteHeader.left

                    width: treeDelegate.width - noteHeader.x
                    wrapMode: Text.WordWrap

                    text: model.display
                    textFormat: Text.MarkdownText
                    rightPadding: treeDelegate.padding
                    bottomPadding: treeDelegate.indent
                }
            }
        }
    }
}
