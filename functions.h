#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QtCore/QCoreApplication>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QTextStream>


class treeHtml {
public:
    QDomDocument tree; // html � ���� ������

    /*!
    * ����������� ����������� ����� ������ � �������
    *\param [in] node - ���� ������
    */
    void postOrderDFS(QDomNode & node);
};

/*!
* �������� QString �� ������������ ������ �����
*\param [out] out - ���������� ������
*/
void getQString(QString & out);

/*!
* ��������� html-�������� �� URL
*\param [in] URL - URL-����� ��� ���������� html
*\param [in] fullFilename - ������ ���, ������������ html-�����
*\return - ���������� true, ���� html ���� ������� ������� � ���������
*/
bool downloadHTML(const QString url, const QString fullFilename);

/*!
* ���������� ����������� html-�������� � xml-�������� ����������� HTML Tidy
* ������� ����, ���������� �������������� � ������ � html-��������, � ����� � ����������� ������
*\param [in] htmlFilename - ������ ��� html-��������
*\param [in] xmlFilename - ������ ��� xml-��������
*\return - ���������� true, ���� ����������� ��������� �������
*/
bool htmlToXml(const QString htmlFilename, const QString xmlFilename);

/*!
* ������� xml-��������
*\param [in] xmlFilename - ������ ��� xml-��������
*\param [out] treeHtml - ������
*\return - ���������� true, ���� ������� ��������� �������
*/
bool parsingXml(const QString xmlFilename, QDomDocument & tree);

/*!
* �������� ����������� ���� � html-�������� �� ����������� �������������� ������ ul-li
*\param [in] tree - ��������, �������������� � ���� ������
*/
void repDuplicateTags(treeHtml & tree);

/*!
* ���������� ����������� xml-�������� � html-�������� ����������� HTML Tidy
*\param [in] xmlFilename - ������ ��� xml-��������
*\param [in] htmlFilename - ������ ��� html-��������
*\return - ���������� true, ���� ����������� ��������� �������
*/
bool xmlToHtml(const QString xmlFilename, const QString htmlFilename);


#endif // FUNCTIONS_H
