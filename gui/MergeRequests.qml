import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

SplitView {
    id: mergeRequests
    required property int currentProject

    orientation: Qt.Vertical

    MRTable {
        id: mrList

        SplitView.minimumHeight: 350

        currentProject: mergeRequests.currentProject
    }

    MergeRequestDiscussions {
        id: discussions

        currentProject: mergeRequests.currentProject
        currentMrId: mrList.currentMrId
        currentMrIid: mrList.currentMrIid
    }
}
