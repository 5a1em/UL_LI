﻿#include "functions.h"

void getQString(QString & out) {
    QTextStream s(stdin);
    out = s.readLine();
}

bool downloadHTML(const QString url, const QString fullFilename) {
    // Загрузка html-разметки
    QNetworkAccessManager manager; // объект для запроса
    QNetworkReply *response = manager.get(QNetworkRequest(QUrl(url))); // выполняем запрос
    QEventLoop event;
    QObject::connect(response,SIGNAL(finished()),&event,SLOT(quit())); // сигнал загрузки
    event.exec();

    // Если не удалось загрузить html
    if(response->error() != QNetworkReply::NoError) {
        throw QString("function dowloadHTML: Unable to access html file at input URL.");
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
        throw QString("function dowloadHTML: Unable to save file.");
    }

    outputHtml.close();
    return true;
}

bool htmlToXml(const QString htmlFilename, const QString xmlFilename) {
    QStringList params; // параметры для запуска xmllint

    params << "-html" << "-xmlout" << htmlFilename << "-output" << xmlFilename;

    if(QProcess::execute("xmllint", params) != QProcess::NormalExit) {
        throw QString("function htmlToXml: Process error. Perhaps in the root folder is missing \"tide.exe\".");
    }

    return true;
}

bool parsingXml(const QString xmlFilename, QDomDocument & tree) {
    QString errorMsg;
    int errorLine, errorColumn;

    // Открыть xml для чтения
    QFile xml(xmlFilename);
    if (!xml.open(QIODevice::ReadOnly))
        throw QString("function parsingXml: Unable to open xml file.");
    // Парсинг xml
    if (!tree.setContent(&xml, &errorMsg, &errorLine, &errorColumn)) {
        xml.close();
        throw QString("function parsingXml: Unable to parsing xml file. Error message: ") + errorMsg +
                " from line " + QString::number(errorLine) + ", column " + QString::number(errorColumn) + ".";
    }
    xml.close();

    return true;
}

void treeHtml::repDuplicateTags(const QStringList & repTags) {

}

void treeHtml::preOrder(QDomNode & node) {
    QQueue<QDomNode*> childrenNodes;
    QDomNode * curNode;

    if(node.isNull() || node.isText())
        return;

    qprint << node.toElement().tagName();

    getChildren(node, childrenNodes);
    for(int i = 0; i < childrenNodes.length(); i++) {
        curNode = childrenNodes.dequeue();
        preOrder(*curNode);
    }
}

void treeHtml::getChildren(QDomNode & node, QQueue<QDomNode *> & children) {
    QDomNode * curChild = new QDomNode();
    // если у узла есть дети
    if(node.hasChildNodes()) {
        // добавить в очередь первого ребенка
        curChild = &(node.firstChild());
        children.enqueue(curChild);

        //пока есть дети
        while(!curChild->nextSibling().isNull()) {
            curChild = curChild->nextSibling();
            children.enqueue(curChild);
        }
    }
}


void treeHtml::insertUL_LI(QVector<QDomNode> duplicateList) {
    // получить родителя
    QDomNode parent = duplicateList[0].parentNode();

    // удалить связь родителя с повторяющися тегами
    for(int i = 0; i < duplicateList.length(); i++) {
        parent.removeChild(duplicateList[i]);
    }

    // создать тег ul
    QDomElement ul;
    ul.setTagName("ul");

    // вставляем тег ul
    parent.appendChild(ul);

    // заменить повторяющиеся теги на li
    for(int i = 0; i < duplicateList.length(); i++) {
        QDomElement li = duplicateList[i].toElement();
        li.setTagName("li");
        duplicateList[i] = li;
    }

    // сделать ul родителем тегов li
    for(int i = 0; i < duplicateList.length(); i++) {

    }
}


























