import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import GPRunner

Rectangle {
    required property TreeView treeView
    required property bool expanded

    readonly property real padding: 5

    id: root

    implicitWidth:  headerLayout.implicitWidth  + padding * 2
    implicitHeight: headerLayout.implicitHeight + padding * 2

    color: discussionHover.hovered ? (discussion.isResolved ? "#3f4e3f" : palette.highlight) : (discussion.isResolved ? "#2d3c2d" : palette.base)

    function canResolve(discussion) {
        return discussion && discussion.isResolvable && discussion.author == gpm.currentUser
    }

    Rectangle {
        height: 1
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: palette.window
        visible: !root.expanded
    }

    RowLayout {
        id: headerLayout

        anchors.fill: parent
        anchors.margins: padding

        ColumnLayout {
            RowLayout {
                TextLinkButton {
                    url: discussion.url
                    text: discussion.isResolvable ? "Thread" : "Comment"
                }
                Label { text: "by" }
                User  { user: discussion.author }
                Label {
                    text: "updated at " + Utility.formatDateTime(discussion.updated)
                    font.pointSize: 9
                }
            }
            RowLayout {
                Label {
                    text: "Created at " + Utility.formatDateTime(discussion.started)
                    visible: discussion.started != discussion.updated
                    font.pointSize: 9
                }
                Label {
                    visible: canResolve(discussion)
                    text: discussion.isResolved ? "[Unresolve]" : "[Resolve]"
                    font.pointSize: 9

                    HoverHandler { cursorShape: Qt.PointingHandCursor }

                    TapHandler {
                        onTapped: {
                            if(discussion.isResolved) discussion.unresolve()
                            else discussion.resolve()
                        }
                    }
                }
            }
        }

        HorizontalSpacer {}

        UnreadMarker {
            discussion: model.discussion
            visible: model.discussion.hasUnreadNotes
        }

        Label { text: "[" + discussion.noteCount + "]" }

        Label {
            id: indicator

            text: root.expanded ? "Hide thread ˄" : "Show thread ˅"

            TapHandler {
                onTapped: treeView.toggleExpanded(row)
            }

            HoverHandler {
                id: discussionHover
                cursorShape: Qt.PointingHandCursor
            }
        }
    }
}
