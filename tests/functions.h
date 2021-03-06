﻿#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define qprint qDebug().nospace().noquote()
#define CORRECT_NUMBER_ARGS 5

#include <iostream>
#include <QtCore/QCoreApplication>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcess>
#include <QDir>
#include <QDebug>
#include <QTextStream>
#include <QVector>
#include <QList>
#include <QQueue>
#include <QSet>
#include <QMap>
#include <QTextCodec>

/*!
* Получает QString из стандартного потока ввода
*\param [out] out - полученная строка
*/
void getQString(QString & out);

/*!
* Windows-1251 to Unicode
*\param [in] inputData - массив входных данных
*\param [out] string - строка
*/
void windows1251ToUnicode(QByteArray & inputData, QString & string);

class InputData {
public:
    /*!
    * Конструктор класса - заполняет поддерживаемые аргументы командной строки
    * param [in] argc - количество аргументов командной строки
    *\param [in] argv - параментры командной строки
    */
    InputData(int argc, char *argv[]);

    /*!
    * Получает html-разметку
    *\param [in] openFrom - имя файла на компьютере или URL-адрес для скачивания html
    *\param [in] outputFilename - имя, сохраняемого html-файла
    *\return - возвращает true, если html была успешно скачана и сохранена
    */
    static bool getHtml(const QString openFrom, const QString outputFilename);

    /*!
    * Производит конвертацию html-разметки в xml-разметку средставами xmllint
    *\param [in] htmlFilename - полное имя html-разметки
    *\param [in] xmlFilename - полное имя xml-разметки
    *\return - возвращает true, если конвертация произошла успешно
    */
    static bool htmlToXml(const QString htmlFilename, const QString xmlFilename);

    /*!
    * Парсинг xml-разметки
    *\param [in] xmlFilename - полное имя xml-разметки
    *\param [out] tree - дерево
    *\return - возвращает true, если парсинг произошел успешно
    */
    static bool parsingXml(const QString xmlFilename, QDomDocument & tree);

    /*!
    * Получить значение флага
    *\param [in] flag - полное имя xml-разметки
    *\param [out] treeHtml - дерево
    *\return - значение флага
    */
    QString getFlagValue(const QString flag);

private:
    QMap<QString, QString> params; // параметры командной строки
    QStringList supportedParams;   // поддерживаемые аргументы командной строки
};

class Ulli {
public:
    /*!
    * Констуктор класса
    *\param [in] tree - разметка, представленная в виде дерева
    */
    Ulli(QDomDocument & tree);

    /*!
    * Заменяет повторяющие теги в html-разметке на конструкцию маркированного списка ul-li
    *\param [in] tree - разметка, представленная в виде дерева
    */
    void repDuplicateTags();

    /*!
    * Сохраняет дерево в формате xml
    *\param [in] xmlFilename - разметка, представленная в виде дерева
    *\return - возвращает true, если xml файл был сохранен
    */
    bool saveXml(QString xmlFilename);

private:
    QStringList supportedTags;       // поддерживаемые замене на конструкцию ul-li теги
    QStringList replaceableUserTags; // пользовательские заменяемые теги
    QStringList replaceableTags;     // заменяемые теги (объединение поддерживаемых и пользовательских тегов)

    const QString supportedTagsFilename = "supportedTags.txt"; // имя файла с поддерживаемыми
    const QString userTagsFilename = "userTags.txt";           // имя файла с пользовательскими заменяемыми тегами

    QDomDocument tree; // html в виде дерева

    /*!
    * Вставить конструкцию маркированного списка ul-li (проверка одноуровенности тегов не осуществяется)
    *\param [in] sequence - список повторяющихся тегов
    */
    void insertUL_LI(QVector<QDomNode> & sequence);

    /*!
    * Рекурсивный префиксный обход дерева в глубину
    *\param [in] node - узел дерева
    */
    void preOrder(QDomNode node);

    /*!
    * Получить список детей узла node
    *\param [in] node - узел
    *\param [out] children - список детей узла
    */
    void getChildren(QDomNode & node, QVector<QDomNode> & children);

    /*!
    * Заменить последовательности тегов на конструкцию маркированного списка ul-li
    *\param [in] nodes - дочерние узлы
    */
    void replaceSequence(QVector<QDomNode> & nodes);

    /*!
    * Исключает из пользовательского списка заменяемых тегов неподдерживаемые теги
    */
    void excludeUnsupportedTags();

    /*!
    * Проверить подвергается ли последовательность дочерних тегов замене на констукцию ul-li
    *\param [in] sequenceTag - тег последовательности
    *\return - возвращает true, если тег последовательности может быть изменен
    */
    bool checkReplaceableTags(const QString sequenceTag);

    /*!
    * Получить теги из файла
    *\param [in] filename - файл с тегами
    *\param [out] tags - теги из файла
    *\return - возвращает false, если не удается открыть файл с пользовательскими тегами
    */
    bool getListOfTags(const QString filename, QStringList & tags);
};

class OutputData {
public:
    /*!
    * Производит конвертацию xml-разметки в html-разметку средставами xmllint
    *\param [in] xmlFilename - полное имя xml-разметки
    *\param [in] htmlFilename - полное имя html-разметки
    *\return - возвращает true, если конвертация произошла успешно
    */
    static bool xmlToHtml(const QString xmlFilename, const QString htmlFilename);

    /*!
    * Удаляет из разметки автоматически сгенерированную xmllint шапку
    *\param [in] htmlFilename - полное имя xml-разметки
    *\return - возвращает false, если не удается открыть файл
    */
    static bool removeXmllintHeader(const QString htmlFilename);
};

#endif // FUNCTIONS_H
