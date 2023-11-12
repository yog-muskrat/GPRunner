import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

SplitView {
    id: mergeRequests
    required property int currentProject

    anchors.fill: parent
    orientation: Qt.Vertical

    MRTable {
        id: mrList

        SplitView.minimumHeight: 350
        //SplitView.preferredHeight: parent.height / 2

        currentProject: mergeRequests.currentProject
    }

    MergeRequestDiscussions {
        id: discussions

        currentProject: mergeRequests.currentProject
        currentMrId: mrList.currentMrId
        currentMrIid: mrList.currentMrIid
    }
}
