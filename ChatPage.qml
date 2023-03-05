import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtTextToSpeech
import QtMultimedia
import QtCore
import Qt.labs.settings
import Intellicute
import core

Page {
    id: root

    property string defaultMessages: JSON.stringify(
                                         root.settings.chat.defaultMessages)
    required property var settings

    signal settingsRequested

    function resetSettings() {
        root.settings = root.defaultSettings
    }

    QtObject {
        id: d

        function sendGptRequest() {
            if (input.text.length > 0) {
                messageModel.addMessage(OpenAIMessage.User, input.text)
                input.text = ""
            }
            gptRequest.execute() // might be too soon -> delay
        }

        function speak(text) {
            if (root.settings.tts.watson.use) {
                watsonTts.text = text
                watsonTts.execute()
            } else {
                tts.say(text)
            }
        }

        function resetChat() {
            messageModel.clearMessages()
            messageModel.jsonString = root.defaultMessages
        }
    }

    TextToSpeech {
        id: tts
        //engine: "speechd"
        locale: Qt.locale(root.settings.tts.locale)
    }

    WatsonTTSRequest {
        id: watsonTts
        accessToken: root.settings.tts.watson.apiKey
        voice: root.settings.tts.watson.voice
        apiURL: root.settings.tts.watson.apiUrl

        onRequestFinished: {
            console.log("Watson TTS finished")
            player.source = ""
            player.source = "file://" + watsonTts.filePath
            player.play()
        }
        onErrorStringChanged: console.log(errorString)
    }

    MediaPlayer {
        id: player
        audioOutput: AudioOutput {
            muted: false
            volume: 1.0
        }
    }

    OpenAIRequest {
        id: whisperRequest
        accessToken: root.settings.openai.accessToken
        model: "whisper-1"
        filePath: recorder.filePath
        onErrorStringChanged: console.log(errorString)
        onStatusChanged: {
            if (status === OpenAIRequest.Success
                    && (generatedText.length > 0)) {
                input.text = generatedText
                d.sendGptRequest()
            }
        }
    }

    AudioRecorder {
        id: recorder
        onRecordingComplete: {
            console.log("Recording complete")
            console.log(recorder.filePath)
            whisperRequest.execute()
        }
    }

    OpenAIRequest {
        id: gptRequest
        accessToken: root.settings.openai.accessToken
        model: root.settings.chat.model
        maxTokens: root.settings.chat.maxTokens
        temperature: root.settings.chat.temperature
        topP: root.settings.chat.topP
        frequencyPenalty: root.settings.chat.frequencyPenalty
        presencePenalty: root.settings.chat.presencePenalty
        messages: messageModel.messages
        onRequestFinished: generatedText => {
                               console.log("GPT request finished")
                               messageModel.addMessage(OpenAIMessage.Assistant,
                                                       generatedText)
                               if (root.settings.tts.autoplay) {
                                   d.speak(generatedText)
                               }
                           }
        onRequestError: errorString => {
                            console.log("GPT request error: " + errorString)
                        }
    }

    OpenAIMessageModel {
        id: messageModel
    }

    Settings {
        id: messageStore
        category: "chat"
        property alias messages: messageModel.jsonString
    }

    Settings {
        id: pathSettings
        category: "paths"
        property alias importPath: importChatDialog.currentFolder
        property alias exportPath: exportChatDialog.currentFolder
    }

    FileDialog {
        id: importChatDialog
        title: qsTr("Import Chat File")
        currentFolder: StandardPaths.writableLocation(
                           StandardPaths.DocumentsLocation)
        acceptLabel: qsTr("Import")
        nameFilters: [qsTr("Chat Files (*.chat.json)")]
        defaultSuffix: "chat.json"
        fileMode: FileDialog.OpenFile
        onAccepted: {
            readFileIo.filePath = importChatDialog.selectedFile
            readFileIo.read()
            if (readFileIo.hasError) {
                console.error(readFileIo.errorString)
            }
        }

        FileIO {
            id: readFileIo
            onTextChanged: {
                if (readFileIo.text) {
                    messageModel.jsonString = readFileIo.text
                }
            }
        }
    }

    FileDialog {
        id: exportChatDialog
        title: qsTr("Export Chat File")
        currentFolder: StandardPaths.writableLocation(
                           StandardPaths.DocumentsLocation)
        acceptLabel: qsTr("Export")
        nameFilters: [qsTr("Chat Files (*.chat.json)")]
        defaultSuffix: ".chat.json"
        fileMode: FileDialog.SaveFile
        onAccepted: {
            writeFileIo.text = messageModel.jsonString
            writeFileIo.filePath = exportChatDialog.selectedFile
            writeFileIo.write()
            if (writeFileIo.hasError) {
                console.error(writeFileIo.errorString)
            }
        }

        FileIO {
            id: writeFileIo
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.singleMargin

        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Rectangle {
                color: "green"
                SplitView.preferredWidth: 150
                clip: true
                visible: false

                ListView {
                    id: sessionView
                    anchors.fill: parent
                    anchors.margins: Style.singleMargin
                    spacing: Style.spacing
                    model: 5
                    delegate: Rectangle {
                        id: item
                        required property int index
                        width: sessionView.width
                        height: 50
                        color: "lightgreen"

                        Text {
                            anchors.centerIn: parent
                            text: qsTr("Chat %1").arg(item.index + 1)
                        }
                    }
                }
            }

            Rectangle {
                SplitView.preferredWidth: 500
                color: "white"
                clip: true

                ChatView {
                    id: chatView
                    anchors.fill: parent
                    anchors.margins: Style.singleMargin
                    model: messageModel
                    ttsActive: watsonTts.status === WatsonTTSRequest.InProgress
                    onSpeak: message => {
                                 d.speak(message)
                             }
                    onRemoveMessage: message => {
                                         messageModel.removeMessage(message)
                                     }
                }
            }
        }

        RowLayout {
            Layout.preferredHeight: 150
            Layout.fillHeight: false

            TextArea {
                id: input
                Layout.fillWidth: true
                Layout.fillHeight: true
                readOnly: gptRequest.status === OpenAIRequest.InProgress
                placeholderText: qsTr("Enter your prompt here")
                wrapMode: TextArea.WordWrap

                background: Rectangle {
                    color: "white"
                    border.color: "lightgray"
                    border.width: 2
                }

                Keys.onReturnPressed: event => {
                                          if (event.modifiers === Qt.ShiftModifier) {
                                              event.accepted = false
                                              return
                                          }

                                          d.sendGptRequest()
                                      }
            }

            ColumnLayout {
                Layout.fillHeight: true

                RecordButton {
                    id: recordButton
                    Layout.fillHeight: true
                    onCheckedChanged: {
                        if (checked) {
                            recorder.start()
                        } else {
                            recorder.stop()
                        }
                    }

                    Shortcut {
                        sequence: "Ctrl+Space"
                        onActivated: recordButton.checked = !recordButton.checked
                    }
                }

                Button {
                    id: sendButton
                    Layout.fillHeight: true
                    text: "\uf1d8"
                    font.family: Style.iconFont
                    font.pixelSize: 24
                    enabled: gptRequest.status !== OpenAIRequest.InProgress
                    onClicked: d.sendGptRequest()
                }
            }
        }

        RowLayout {

            Button {
                id: resetButton
                text: qsTr("Reset")
                onClicked: {
                    d.resetChat()
                }

                Shortcut {
                    sequence: "Ctrl+R"
                    onActivated: resetButton.clicked()
                }
            }

            Button {
                id: importChatButton
                text: qsTr("Import Chat")

                onClicked: {
                    importChatDialog.open()
                }

                Shortcut {
                    sequence: "Ctrl+O"
                    onActivated: importChatButton.clicked()
                }
            }

            Button {
                id: exportChatButton
                text: qsTr("Export Chat")
                onClicked: {
                    exportChatDialog.open()
                }

                Shortcut {
                    sequence: "Ctrl+S"
                    onActivated: exportChatButton.clicked()
                }
            }

            Button {
                id: stopTtsButton
                text: qsTr("Stop Playback")
                visible: player.playbackState === MediaPlayer.PlayingState
                onClicked: {
                    player.stop()
                }
            }

            Label {
                id: statusLabel
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: gptRequest.status
                      === OpenAIRequest.InProgress ? qsTr("Request running") : qsTr(
                                                         "Idle")
                elide: Text.ElideRight
            }

            ToolButton {
                id: settingsButton
                text: "\uf0ad"
                font.pixelSize: 16
                font.family: "awesome"
                onClicked: root.settingsRequested()
            }
        }

        Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: gptRequest.errorString
            visible: gptRequest.status === OpenAIRequest.Error
            elide: Text.ElideRight
            color: "red"
        }
    }
}
