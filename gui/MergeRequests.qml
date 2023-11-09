import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

SplitView {
    MRTable {
        id: mrList

        SplitView.minimumWidth: 500
        implicitWidth: 750
    }

    MergeRequestDiscussions {
        id: discussions

        implicitWidth: 250

        currentMR: mrList.currentMR
    }
}
