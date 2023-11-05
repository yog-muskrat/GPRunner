import QtQuick
import QtCharts
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
	ColumnLayout {
		anchors.fill: parent

		RowLayout {
			Layout.fillWidth: true
			//Layout.alignment: Qt.AlignVTop

			Text { text: "From" }
			TextInput { id: from }
			Text { text: "To" }
			TextInput { id: to }
		}

//		ChartView {
//			id: chart
//
//			Layout.fillWidth:  true
//			Layout.fillHeight: true
//		}
	}
}