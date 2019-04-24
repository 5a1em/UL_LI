#include "functions.h"

void getQString(QString & out) {
    QTextStream s(stdin);
    out = s.readLine();
}

bool downloadHTML(const QString url, const QString fullFilename) {
    // �������� html
    QNetworkAccessManager manager; // ������ ��� �������
    QNetworkReply *response = manager.get(QNetworkRequest(QUrl(url))); // ��������� ������
    QEventLoop event;
    QObject::connect(response,SIGNAL(finished()),&event,SLOT(quit())); // ������ ��������
    event.exec();

    // ���� �� ������� ��������� html
    if(response->error() != QNetworkReply::NoError) {
        return false;
    }

    // ��������� ��������� ������ � QString
    QString html = response->readAll();

    // C��������� html
    QFile outputHtml(fullFilename);

    // ���� ������� ������� ���� ��� ������
    if(outputHtml.open(QIODevice::WriteOnly)) {
        QTextStream out(&outputHtml);
        out << html;
    }
    else {
        outputHtml.close();
        return false;
    }

    outputHtml.close();
    return true;
}

bool htmlToXml(const QString htmlFilename, const QString xmlFilename) {
    QProcess process;   // ������� ����� �������
    QStringList params; // ��������� ��� ������� tidy.exe

    params << "-config" << "config.txt" << "-o" << xmlFilename << "-asxml" << htmlFilename;

    // ��������� ������� � �������� ����
    process.setProcessChannelMode(QProcess::MergedChannels);
    // ������ ��������
    process.start("tidy.exe", params);
    // ����� ���� ������� �� ����������
    process.waitForFinished(-1);

    return process.exitStatus() == QProcess::NormalExit;
}

bool parsingXml(const QString xmlFilename, QDomDocument & tree) {
    // ������� xml ��� ������
    QFile xml(xmlFilename);
    if (!xml.open(QIODevice::ReadOnly))
        return false;
    // ������� xml
    if (!tree.setContent(&xml)) {
        xml.close();
        return false;
    }
    xml.close();

    return true;
}

void repDuplicateTags(QDomDocument & tree) {

}

void postOrderDFS(QDomNode & node) {
    // ���� ������� ���� ������ �� ���������� ����� �������� ����
    if(node.isNull()) {
        return;
    }

    // ������ ������ (�����) ���������
    QDomNode firstChild = node.firstChild();
    postOrderDFS(firstChild);

    qDebug() << node.toElement().tagName();

    // ������ �������� ���������(������)
    QDomNode nextChild = node.nextSibling();
    postOrderDFS(nextChild);

}













