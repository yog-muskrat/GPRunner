import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
  clip: true

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
        selectionModel: ItemSelectionModel {
        }

        columnWidthProvider: function (column) {
          let ew = explicitColumnWidth(column);
          let iw = implicitColumnWidth(column);
          let hw = header.implicitColumnWidth(column);
          return max(hw, iw, ew);
        }

        delegate: Rectangle {
          implicitWidth: itemText.implicitWidth + urlButton.implicitWidth
          implicitHeight: itemText.implicitHeight
          color: row == mrs.currentRow ? palette.highlight : ((row % 2) == 0 ? palette.alternateBase : palette.base)

          Text {
            id: urlButton
            visible: column == 0
            padding: 5
            text: "🌐"

            MouseArea {
              anchors.fill: parent
              cursorShape: Qt.PointingHandCursor
              onClicked: Qt.openUrlExternally(model.url)
            }
          }

          Text {
            id: itemText

            anchors.fill: parent
            anchors.leftMargin: urlButton.visible ? urlButton.width - 5 : 0
            padding: 5
            text: model.display
            font.bold: model.font.bold
            color: mrs.currentRow ? palette.highlightedText : palette.text
            ToolTip.delay: 500
            ToolTip.timeout: 3000
            ToolTip.text: model.toolTip
            ToolTip.visible: model.toolTip ? tooltipMA.containsMouse : false

            MouseArea {
              id: tooltipMA
              visible: model.toolTip
              anchors.fill: parent
              hoverEnabled: true
            }
          }
        }
      }
    }
  }
}
