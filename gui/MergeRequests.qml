import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

SplitView {
    id: mergeRequests
    required property var currentProject

    orientation: Qt.Vertical

    MRTable {
        id: mrList

        SplitView.minimumHeight: 350

        currentProject: mergeRequests.currentProject
    }

    MergeRequestDiscussions {
        currentMR: mrList.currentMR
    }
}
