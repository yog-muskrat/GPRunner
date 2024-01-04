import QtQuick
import QtQuick.Controls

Label {
	property int projectId
	property var mr
	property string discussionId

	text: "●"
	color: "#B21818"

	HoverHandler { cursorShape: Qt.PointingHandCursor }

	TapHandler {
		onTapped: {
			if(projectId > 0 && mr && discussionId) gpm.markDiscussionsRead(projectId, mr.iid, discussionId)
			else if(mr)                             mr.markDiscussionsRead()
			else if(projectId > 0)                  gpm.markDiscussionsRead(projectId)
		}
	}
}
