import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "Utility.js" as Utility

Item {
    required property int currentProject

    property int currentMrId: 0
    property int currentMrIid: 0

    id: mrTable

    clip: true

    onCurrentProjectChanged: {
        currentMrId = 0
        currentMrIid = 0
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
                id: mrs

                anchors.fill: parent

                focus: true
                clip: true

                model: gpm.mrModel
                selectionModel: ItemSelectionModel {}
                editTriggers: TableView.DoubleTapped

                columnWidthProvider: Utility.calcColumnWidth.bind(this, header)

                delegate: MergeRequestTableDelegate {
                    currentProject: mrTable.currentProject

                    onMrSelected: function(id, iid){ 
                        mrTable.currentMrId = id
                        mrTable.currentMrIid = iid
                    }
                }
            }
        }
    }
}
