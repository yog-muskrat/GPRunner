import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

SplitView {
    required property int currentProject

    id: mergeRequests

    MRTable {
        id: mrList

        SplitView.minimumWidth: 500
        implicitWidth: 750

        currentProject: mergeRequests.currentProject
    }

    MergeRequestDiscussions {
        id: discussions

        implicitWidth: 250

        currentProject: mergeRequests.currentProject
        currentMrId: mrList.currentMrId
        currentMrIid: mrList.currentMrIid
    }
}
