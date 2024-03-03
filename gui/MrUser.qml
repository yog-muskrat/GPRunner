import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

User {
    property var mr

    id: root

    function currentUserCanApprove() {
        return root.user == gpm.currentUser && root.mr.userCanApprove(root.user)
    }

    function currentUserApproved() {
        return root.user == gpm.currentUser && root.mr.isApprovedBy(root.user)
    }

    Label {
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
