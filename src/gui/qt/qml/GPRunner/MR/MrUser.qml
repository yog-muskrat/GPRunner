import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import GPRunner

RowLayout {
    property alias user: userItem.user
    property alias size: userItem.size
    property alias editable: userItem.editable

    property alias font: userItem.font
    property alias showLabel: userItem.showLabel

    property var mr

    signal userSelected(var newUser)
    signal userCleared()

    id: root

    spacing: 5

    function currentUserCanApprove() {
        return userItem == gpm.currentUser && root.mr.userCanApprove(userItem)
    }
    
    function currentUserApproved() {
        return userItem == gpm.currentUser && root.mr.isApprovedBy(userItem)
    }

    User {
        id: userItem

        onUserSelected: (newUser) => { root.userSelected(newUser) }
        onUserCleared: root.userCleared()
    }

    Label {
        id: check

        text: mr.isApprovedBy(user) ? "☑" : currentUserCanApprove() ? "☐" : ""
        color: currentUserCanApprove() ? (currentUserApproved() ? "green" : "yellow") : palette.text
    
        DefaultToolTip {
            toolTipText: currentUserCanApprove() ? (currentUserApproved() ? "Unapprove" : "Approve") : ""
        }
    
        HoverHandler {
            enabled: currentUserCanApprove()
            cursorShape: Qt.PointingHandCursor
        }
    
        TapHandler {
            enabled: currentUserCanApprove()
            onTapped: currentUserApproved() ? mr.unapprove() : mr.approve()
        }
    }
}
