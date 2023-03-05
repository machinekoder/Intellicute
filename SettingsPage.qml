import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Intellicute
import core

Page {
    id: root

    required property var settings

    signal cancel()
    signal accept()
    signal reset()

    function applySettings() {
        root.settings.tts.autoplay = autoplayCheckbox.checked
        root.settings.tts.watson.use = useWatsonCheckbox.checked
        root.settings.tts.watson.voice = voiceComboBox.currentText
        root.settings.tts.locale = localeComboBox.currentText
        root.settings.tts.watson.apiKey = watsonApiKeyTextField.text
        root.settings.tts.watson.apiUrl = watsonApiUrlTextField.text
        root.settings.chat.defaultMessages = JSON.parse(messageModel.jsonString)
        root.settings.chat.temperature = temperatureSlider.value
        root.settings.chat.topP = topPSlider.value
        root.settings.chat.frequencyPenalty = frequencyPenaltySlider.value
        root.settings.chat.presencePenalty = presencePenaltySlider.value
        root.settings.chat.maxTokens = maxTokensSpinBox.value
        root.settings.openai.accessToken = openAiAccessTokenTextField.text
    }

    QtObject {
        id: d
        readonly property var watsonVoices: ["de-DE_DieterV3Voice", "de-DE_BirgitV3Voice", "en-US_AllisonV3Voice", "en-US_LisaV3Voice", "en-US_MichaelV3Voice", "es-ES_EnriqueV3Voice", "es-ES_LauraV3Voice", "es-LA_SofiaV3Voice", "es-US_SofiaV3Voice", "fr-FR_ReneeV3Voice", "it-IT_FrancescaV3Voice", "ja-JP_EmiV3Voice", "ko-KR_YoungmiV3Voice", "pt-BR_IsabelaV3Voice", "zh-CN_LiNaVoice", "zh-CN_WangWeiVoice", "zh-CN_ZhangJingVoice"]
        readonly property var openAiModels: ["gpt-3.5-turbo"]
    }

    component ValueSlider: RowLayout {
        property alias value: control.value
        property alias slider: control

        Label {
            text: control.from
        }

        Slider {
            id: control
            Layout.fillWidth: true
            from: 0
            to: 1
            stepSize: 0.1
            handle: Rectangle {
                x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
                y: control.topPadding + control.availableHeight / 2 - height / 2
                implicitWidth: 32
                implicitHeight: 32
                radius: width / 2.0
                color: control.pressed ? "#f0f0f0" : "#f6f6f6"
                border.color: "#bdbebf"
                
                Label {
                    anchors.centerIn: parent
                    text: control.value.toFixed(1)
                }
            }
        }

        Label {
            text: control.to
        }
    }

    component HidableTextField: RowLayout {
        property alias text: control.text
        property alias textField: control
        property alias hidden: button.checked

        TextField {
            id: control
            Layout.fillWidth: true
            echoMode: !button.checked ? TextInput.Password : TextInput.Normal
        }

        ToolButton {
            id: button
            text: control.echoMode == TextInput.Normal ? "\uf06e" : "\uf070"
            font.family: "FontAwesome"
            checkable: true
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.doubleMargin

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            GroupBox {
                Layout.fillWidth: true
                Layout.preferredHeight: 300
                title: qsTr("Default Messages")
                clip: true

                ChatView {
                    anchors.fill: parent
                    anchors.margins: Style.singleMargin
                    model: OpenAIMessageModel {
                        id: messageModel
                        jsonString: JSON.stringify(root.settings.chat.defaultMessages)
                    }
                }
            }

            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Chat Settings")

                GridLayout {
                    anchors.fill: parent
                    columns: 2

                    Label {
                        text: qsTr("Temperature")
                    }

                    ValueSlider {
                        id: temperatureSlider
                        Layout.fillWidth: true
                        slider.value: root.settings.chat.temperature
                    }
                    
                    Label {
                        text: qsTr("Top P")
                    }

                    ValueSlider {
                        id: topPSlider
                        Layout.fillWidth: true
                        slider.value: root.settings.chat.topP
                    }

                    Label {
                        text: qsTr("Frequency Penalty")
                    }

                    ValueSlider {
                        id: frequencyPenaltySlider
                        Layout.fillWidth: true
                        slider.value: root.settings.chat.frequencyPenalty
                    }

                    Label {
                        text: qsTr("Presence Penalty")
                    }

                    ValueSlider {
                        id: presencePenaltySlider
                        Layout.fillWidth: true
                        slider.value: root.settings.chat.presencePenalty
                    }

                    Label {
                        text: qsTr("Max Tokens")
                    }

                    SpinBox {
                        id: maxTokensSpinBox
                        value: root.settings.chat.maxTokens
                        from: 1
                        to: 2048
                        editable: true
                    }

                    Label {
                        text: qsTr("OpenAI API Access Token")
                    }

                    HidableTextField {
                        id: openAiAccessTokenTextField
                        Layout.fillWidth: true
                        text: root.settings.openai.accessToken
                    }
                }
            }

            GroupBox {
                Layout.fillWidth: true
                title: qsTr("TTS Settings")

                GridLayout {
                    anchors.fill: parent
                    columns: 2

                CheckBox {
                    id: autoplayCheckbox
                    Layout.columnSpan: 2
                    text: qsTr("Auto Speak")
                    checked: root.settings.tts.autoplay
                }

            Label {
                text: qsTr("TTS Locale")
            }

            ComboBox {
                id: localeComboBox
                model: ["de", "en", "es", "fr", "it", "ja", "ko", "pt", "zh"]
                currentIndex: model.indexOf(root.settings.tts.locale)
            }

            GroupBox {
                label: CheckBox {
                    id: useWatsonCheckbox
                    text : qsTr("Use Watson TTS")
                    checked: root.settings.tts.watson.use
                }
                Layout.columnSpan: 2
                Layout.fillWidth: true

                GridLayout {
                    anchors.fill: parent
                    enabled: useWatsonCheckbox.checked
                    columns: 2

                    Label {
                        text: qsTr("Voice")
                    }

                    ComboBox {
                        id: voiceComboBox
                        Layout.fillWidth: true
                        model: d.watsonVoices
                        currentIndex: d.watsonVoices.indexOf(root.settings.tts.watson.voice)
                    }
                    
                    Label {
                        text: qsTr("IBM Watson API Key")
                    }

                    HidableTextField {
                        id: watsonApiKeyTextField
                        Layout.fillWidth: true
                        text: root.settings.tts.watson.apiKey
                    }

                    Label {
                        text: qsTr("IBM Watson API URL")
                    }

                    HidableTextField {
                        id: watsonApiUrlTextField
                        Layout.fillWidth: true
                        text: root.settings.tts.watson.apiUrl
                    }
                }
            }
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }

        RowLayout {

            Button {
                id: rejectButton
                text: qsTr("Cancel")
                onClicked: {
                    root.cancel()
                }
            }

            Button {
                id: resetButton
                text: qsTr("Reset to Defaults")
                onClicked: {
                    root.reset()
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                id: closeButton
                text: qsTr("Apply")
                onClicked: {
                    root.applySettings()
                    root.accept()
                }
            }
        }
    }
}
