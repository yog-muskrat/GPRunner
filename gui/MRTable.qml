import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "Utility.js" as Utility

Item {
    required property var currentProject

    property var currentMR: null

    id: root

    clip: true

    onCurrentProjectChanged: {
        currentMR = null
        if(currentProject) {
            mrs.model.sourceModel.setProject(currentProject)
        }
        else {
            mrs.model.sourceModel.clear()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        HorizontalHeaderView {
            id: header
            syncView: mrs
            Layout.fillWidth: true
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            TableView {
                signal mrSelected(mr: var)

                id: mrs

                anchors.fill: parent

                focus: true
                clip: true

                model: gpm.getMRModel()
                selectionModel: ItemSelectionModel { }

                editTriggers: TableView.DoubleTapped

                columnWidthProvider: Utility.calcColumnWidth.bind(this, header)

                delegate: MergeRequestTableDelegate { }

                onMrSelected: function (mr) { root.currentMR = mr }
            }
        }
    }
}
