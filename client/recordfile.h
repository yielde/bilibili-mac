#ifndef RECORDFILE_H
#define RECORDFILE_H

#include "mainwindow.h"
#include <QFile>
#include <QJsonObject>
#include <QString>

class NetWorkInfo;

class RecordFile {
public:
    RecordFile(const QString& path);
    ~RecordFile();
    bool DumpJson(QJsonObject obj);
    bool LoadJson();

    bool DumpStream();
    bool LoadStream();

    static QJsonObject LoadNotRecord(QByteArray text);
    static QByteArray DumpNotRecord(QJsonObject obj);

public:
    NetWorkInfo* networinfo;

private:
    QFile m_file;
    QJsonObject m_obj;
};

#endif // RECORDFILE_H
