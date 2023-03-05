#include "awesome_chatgpt_prompts.h"

AwesomeChatGptPrompts::AwesomeChatGptPrompts(QObject *parent)
    : QObject(parent)
{

}

AwesomeChatGptPrompts::~AwesomeChatGptPrompts()
{

}

// TODO
struct MarkdownContent {
    QString title;
    QString author;
    QString referenceUrl;
    QString codeBlock;
};

void parseMarkdownFile(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString line;
    bool startParsing = false;
    MarkdownContent content;

    while (!in.atEnd()) {
        line = in.readLine();

        if (line.startsWith("# Prompts")) {
            startParsing = true;
            continue;
        }

        if (startParsing) {
            if (line.startsWith("## ")) {
                // found a new section, save the previous content
                if (!content.title.isEmpty()) {
                    // do something with the extracted content
                    qDebug() << "Title:" << content.title;
                    qDebug() << "Author:" << content.author;
                    qDebug() << "Reference URL:" << content.referenceUrl;
                    qDebug() << "Code Block:" << content.codeBlock;
                }
                // start a new content section
                content = MarkdownContent();
                content.title = line.mid(3);
            } else if (line.startsWith("Contributed by:")) {
                content.author = line.mid(16, line.indexOf("]") - 16);
                content.referenceUrl = line.mid(line.indexOf("(") + 1, line.indexOf(")") - line.indexOf("(") - 1);
            } else if (line.startsWith("```")) {
                // found a code block, extract the content
                while (!in.atEnd()) {
                    line = in.readLine();
                    if (line.startsWith("```"))
                        break;
                    content.codeBlock += line + "\n";
                }
            }
        }
    }

    // save the last content section
    if (!content.title.isEmpty()) {
        // do something with the extracted content
        qDebug() << "Title:" << content.title;
        qDebug() << "Author:" << content.author;
        qDebug() << "Reference URL:" << content.referenceUrl;
        qDebug() << "Code Block:" << content.codeBlock;
    }

    file.close();
}