import QtQuick
//import QtCharts
import QtQuick.Layouts
import QtQuick.Controls

import GPRunner

Rectangle {
    required property var currentProject

    color: palette.window

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            Layout.leftMargin: 5

            Label {
                text: "From"
            }
            DatePicker {
                id: from
                text: new Date().toLocaleDateString(Qt.locale("ru_RU"), Locale.ShortFormat)
            }
            Label {
                text: "To"
            }
            DatePicker {
                id: to
                text: new Date().toLocaleDateString(Qt.locale("ru_RU"), Locale.ShortFormat)
            }
            Button {
                text: "Load"
            }
        }

        //    ChartView {
        //      id: chart
        //
        //      Layout.fillWidth: true
        //      Layout.fillHeight: true
        //
        //      antialiasing: true
        //
        //      LineSeries {
        //        name: "Line"
        //        XYPoint { x: 0; y: 0 }
        //        XYPoint { x: 1.1; y: 2.1 }
        //        XYPoint { x: 1.9; y: 3.3 }
        //        XYPoint { x: 2.1; y: 2.1 }
        //        XYPoint { x: 2.9; y: 4.9 }
        //        XYPoint { x: 3.4; y: 3.0 }
        //        XYPoint { x: 4.1; y: 3.3 }
        //      }
        //    }
    }
}
