import QtQuick
import QtQuick.Controls

Label {
	property int projectId
	property var mr
	property var discussion

	text: "●"
	color: "#B21818"

	HoverHandler { cursorShape: Qt.PointingHandCursor }

	TapHandler {
		onTapped: {
			if(discussion)         discussion.markRead()
			else if(mr)            mr.markDiscussionsRead()
			else if(projectId > 0) gpm.markDiscussionsRead(projectId)
		}
	}
}
