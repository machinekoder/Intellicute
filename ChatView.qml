import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Intellicute
import core

ListView {
    id: root

    signal speak(message: string)
    signal removeMessage(message: OpenAIMessage)

    property bool ttsEnabled: true
    property bool ttsActive: false

    component FontButton: RoundButton {
        implicitWidth: 32
        implicitHeight: implicitWidth
        text: "\uf0c5"
        font.pixelSize: 16
        font.family: Style.iconFont
    }

    spacing: Style.spacing
    
    delegate: Rectangle {
        id: item
        required property OpenAIMessage message
        width: root.width
        height: Math.max(messageText.implicitHeight, copyButton.implicitHeight) + 20
        radius: 10
        color: item?.message?.role === OpenAIMessage.User ? "lightblue" : (item?.message?.role === OpenAIMessage.System ? "orange" : "lightgreen")
        border.color: color.darker(0.8)
        border.width: 1

        RowLayout {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: Style.doubleMargin

            TextEdit {
                id: messageText
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: item?.message?.content ?? ""
                textFormat: markdownCheckbox.checked ? TextEdit.MarkdownText : TextEdit.PlainText
                wrapMode: TextEdit.WordWrap
                verticalAlignment: TextEdit.AlignVCenter
                onEditingFinished: {
                    item.message.content = messageText.text
                }
            }

            RowLayout {
                id: buttonLayout
                Layout.alignment: Qt.AlignBottom
                opacity: mouseArea.containsMouse ? 1.0 : 0.0

                Behavior on opacity { NumberAnimation { duration: 250 } }

                FontButton {
                    id: copyButton
                    text: "\uf0c5"
                    onClicked: {
                        messageText.selectAll()
                        messageText.copy()
                        messageText.deselect()
                    }
                }

                FontButton {
                    id: playButton
                    visible: (item?.message?.role === OpenAIMessage.Assistant) && root.ttsEnabled
                    enabled: !root.ttsActive
                    text: "\uf04b"
                    onClicked: root.speak(messageText.getText(0, messageText.length))
                }

                FontButton {
                    id: deleteButton
                    text: "\uf1f8"
                    onClicked: {
                        root.model.removeMessage(item.message)
                    }
                }
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            hoverEnabled: true
            onClicked: contextMenu.popup()
        }

        Menu {
            id: contextMenu

            MenuItem {
                text: qsTr("Copy Selection")
                onClicked: {
                    messageText.copy()
                }
            }

            MenuItem {
                text: qsTr("Copy All")
                onClicked: {
                    messageText.selectAll()
                    messageText.copy()
                    messageText.deselect()
                }
            }

            MenuSeparator {}

            CheckBox {
                id: markdownCheckbox
                text: qsTr("Render Markdown")
                checked: true
                onClicked: contextMenu.close()
            }
        }
    }

    ColumnLayout {
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        FontButton {
            id: addButton
            text: "\u002b"
            onClicked: {
                addMessageMenu.popup()
            }

            Menu {
                id: addMessageMenu
                MenuItem {
                    text: qsTr("User")
                    onClicked: {
                        root.model.addMessage(OpenAIMessage.User, "")
                    }
                }
                MenuItem {
                    text: qsTr("Assistant")
                    onClicked: {
                        root.model.addMessage(OpenAIMessage.Assistant, "")
                    }
                }
                MenuItem {
                    text: qsTr("System")
                    onClicked: {
                        root.model.addMessage(OpenAIMessage.System, "")
                    }
                }
            }
        }
    }

    // move to bottom
    onCountChanged: {
        if (count > 0) {
            root.positionViewAtEnd()
        }
    }

    add: Transition {
        NumberAnimation { properties: "opacity"; from: 0; to: 1.0; duration: 500 }
    }

    remove: Transition {
        NumberAnimation { properties: "opacity"; to: 0; duration: 500 }
    }

}
