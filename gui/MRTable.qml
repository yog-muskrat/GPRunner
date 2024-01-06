import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "Utility.js" as Utility

Item {
    required property var currentProject

    property var currentMR: null

    id: mrTable

    clip: true

    onCurrentProjectChanged: { currentMR = null }

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

                model: gpm.getMRModel()
                selectionModel: ItemSelectionModel {}
                editTriggers: TableView.DoubleTapped

                columnWidthProvider: Utility.calcColumnWidth.bind(this, header)

                delegate: MergeRequestTableDelegate {
                    onMrSelected: function(mr) { mrTable.currentMR = mr }
                }
            }
        }
    }
}
