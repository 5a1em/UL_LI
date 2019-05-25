﻿#include "functions.h"

void getQString(QString & out) {
    QTextStream s(stdin);
    out = s.readLine();
}

bool inputData::downloadHTML(const QString url, const QString fullFilename) {
    // Загрузка html-разметки
    QNetworkAccessManager manager; // объект для запроса
    QNetworkReply *response = manager.get(QNetworkRequest(QUrl(url))); // выполняем запрос
    QEventLoop event;
    QObject::connect(response,SIGNAL(finished()),&event,SLOT(quit())); // сигнал загрузки
    event.exec();

    // Если не удалось загрузить html
    if(response->error() != QNetworkReply::NoError) {
        throw QString("function dowloadHTML: Unable to access html file at input URL");
    }

    // Поместить скачанные данные в QString
    QString html = response->readAll();

    // Cохранение html
    QFile outputHtml(fullFilename);

    // Если удалось открыть файл для записи
    if(outputHtml.open(QIODevice::WriteOnly)) {
        QTextStream out(&outputHtml);
        out << html;
    }
    else {
        outputHtml.close();
        throw QString("function dowloadHTML: Unable to save file");
    }

    outputHtml.close();
    return true;
}

bool inputData::htmlToXml(const QString htmlFilename, const QString xmlFilename) {
    QStringList params; // параметры для запуска xmllint

    params << "-html" << "-xmlout" << htmlFilename << "-output" << xmlFilename;

    if(QProcess::execute("xmllint", params) != QProcess::NormalExit) {
        throw QString("function htmlToXml: Unable to create process");
    }

    return true;
}

bool inputData::parsingXml(const QString xmlFilename, QDomDocument & tree) {
    QString errorMsg;
    int errorLine, errorColumn;

    // Открыть xml для чтения
    QFile xml(xmlFilename);
    if (!xml.open(QIODevice::ReadOnly))
        throw QString("function parsingXml: Unable to open xml file");
    // Парсинг xml
    if (!tree.setContent(&xml, &errorMsg, &errorLine, &errorColumn)) {
        xml.close();
        throw QString("function parsingXml: Unable to parsing xml file. Error message: ") + errorMsg +
                " from line " + QString::number(errorLine) + ", column " + QString::number(errorColumn);
    }
    xml.close();

    return true;
}

bool inputData::handlerCmdPatams(int argc, char *argv[], QString & url, QString & downloadFilename, QString & inputFile, QString & outputFile) {

}

ulli::ulli(QDomDocument & tree) {
    this->tree = tree;

    // добавление поддерживаемых замене на конструкцию ul-li тегов
    supportedTags << "blockquote";
    supportedTags << "div";
    supportedTags << "h1";
    supportedTags << "h2";
    supportedTags << "h3";
    supportedTags << "h4";
    supportedTags << "h5";
    supportedTags << "h6";
    supportedTags << "h7";
    supportedTags << "p";
    supportedTags << "a";
    supportedTags << "abbr";
    supportedTags << "acronym";
    supportedTags << "h12";
    supportedTags << "b";
    supportedTags << "cite";
    supportedTags << "code";
    supportedTags << "dfn";
    supportedTags << "em";
    supportedTags << "i";
    supportedTags << "q";
    supportedTags << "s";
    supportedTags << "samp";
    supportedTags << "span";
    supportedTags << "strike";
    supportedTags << "strong";
    supportedTags << "sub";
    supportedTags << "sup";
    supportedTags << "textarea";
    supportedTags << "tt";
    supportedTags << "u";
}

void ulli::repDuplicateTags(const QStringList & repTagsUser) {
    // получить узел с тегом body
    QDomNode root = tree.elementsByTagName("body").at(0);

    // получить список заменяемых тегов
    excludeUnsupportedTags(repTagsUser);

    // Обойти дерево и заменить повторяемые теги
    preOrder(root);
}

void ulli::preOrder(QDomNode node) {
    QVector<QDomNode> children; // cписок детей

    // если узел нулевой то прекратить обход этого узла
    if(node.isNull())
        return;

    qprint << node.toElement().tagName();

    // получить детей узла
    if(node.hasChildNodes()) {
        getChildren(node, children);
    }

    // найти и заменить последовательности повторяющихся тегов
    if(children.length() > 1) {
        replaceSequence(children);
    }

    // продолжить обход дерева
    for(int i = 0; i < children.length(); i++) {
        preOrder(children[i]);
    }

}

void ulli::getChildren(QDomNode & node, QVector<QDomNode> & children) {
    QDomNode child = node.firstChild(); // текущий ребенок

    // пока у узла есть дети
    while(!child.isNull()) {
        // если ребенок не текст
        if(!child.isText()) {
            // добавить ребенка в список детей
            children.append(child);
        }
        // перейти к следующему ребенку
        child = child.nextSibling();
    }
}

void ulli::replaceSequence(QVector<QDomNode> & nodes) {
    QVector<QDomNode> sequence;

    sequence.append(nodes[0]);
    QString sequenceTag = nodes[0].toElement().tagName();

    for(int i = 1; i < nodes.length(); i++) {
        // если последовательность продолжается
        if(nodes[i].toElement().tagName() == sequenceTag) {
            sequence.append(nodes[i]);
        } // если последовательность прервалась или не началась
        else {
            // если последовательность прервалась и последовательность подвергается замене
            if(sequence.length() > 1 && checkReplacableTags(sequenceTag)) {
                insertUL_LI(sequence);
            }
            // начать новую последовательность
            sequence.clear();
            sequence.append(nodes[i]);
            sequenceTag = nodes[i].toElement().tagName();
        }
    }

    // если последний дочерний узел продолжает последовательность и последовательность подвергается замене
    if(sequence.length() > 1 && checkReplacableTags(sequenceTag)) {
        insertUL_LI(sequence);
    }
}

void ulli::insertUL_LI(QVector<QDomNode> & sequence) {
    if(sequence.isEmpty()) {
        return;
    }

    QDomNode parent = sequence[0].parentNode();

    // создать тег ul
    QDomNode ul = tree.createElement("ul");

    for(int i = 0; i < sequence.length(); i++) {
        // удалить связь узла с родителем
        parent.removeChild(sequence[i]);
        // добавить связь узла с ul
        ul.appendChild(sequence[i]);
        // заменить тег узла на li
        sequence[i].toElement().setTagName("li");
    }

    QDomNode beforeNode = sequence[0].previousSibling();
    // добавить связь родителя с тегом ul
    parent.insertBefore(ul, beforeNode);
}

bool ulli::checkReplacableTags(const QString sequenceTag) {
    return replaceableTags.contains(sequenceTag);
}

void ulli::excludeUnsupportedTags(const QStringList & repTagsUser) {
    replaceableTags = (QSet<QString>::fromList(supportedTags) & QSet<QString>::fromList(repTagsUser)).toList();

    if(replaceableTags.length() < repTagsUser.length()) {
        qprint << "Warning: some input tags are not supported (appendix \"A\")";
    }
}

void ulli::printTree(QDomNode node) {
    QVector<QDomNode> children;

    if(node.isNull())
        return;

    qprint << node.toElement().tagName();

    if(node.hasChildNodes()) {
        getChildren(node, children);
    }

    for(int i = 0; i < children.length(); i++) {
        preOrder(children[i]);
    }
}

bool ulli::saveXml(QString xmlFilename) {
    // Открыть xml для записи
    QFile xml(xmlFilename);
    if (!xml.open(QIODevice::WriteOnly))
        throw QString("function saveXml: Unable to create/write xml file");
    xml.write(tree.toByteArray());
    xml.close();

    return true;
}

bool outputData::xmlToHtml(const QString xmlFilename, const QString htmlFilename) {
    QStringList params; // параметры для запуска xmllint

    params << "-html" << "-htmlout" << xmlFilename << "-output" << htmlFilename;

    if(QProcess::execute("xmllint", params) != QProcess::NormalExit) {
        throw QString("function xmlToHtml: Unable to create process");
    }

    return true;
}
