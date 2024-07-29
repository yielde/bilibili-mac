#include "recordfile.h"

#include <QDataStream>
#include <QFile>
#include <QJsonParseError>

RecordFile::RecordFile(const QString& path)
    : m_file(path)

{
}

RecordFile::~RecordFile()
{
    m_file.close();
}

bool RecordFile::DumpJson(QJsonObject obj)
{
    m_file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QByteArray byte_array = QJsonDocument(obj).toJson(QJsonDocument::Indented);
    qDebug() << "Dump: " << byte_array;
    m_file.resize(0);
    m_file.seek(0);
    m_file.write(byte_array);
    m_file.close();
    return true;
}

bool RecordFile::LoadJson()
{
    if (!m_file.open(QIODevice::ReadOnly)) {
        qDebug() << "文件不存在";
        return false;
    }
    QByteArray byte_array = m_file.readAll();
    qsizetype size = byte_array.size();
    if (size <= 0) {
        qDebug() << "Load: " << byte_array;
        return false;
    }
    QJsonParseError json_err;
    QJsonDocument json_doc = QJsonDocument::fromJson(byte_array, &json_err);
    if (json_err.error != QJsonParseError::NoError) {
        qDebug() << "Load json error: " << json_err.errorString();
        return false;
    }

    if (json_doc.isObject()) {
        m_obj = json_doc.object();
    } else {
        qDebug() << "Not JsonObject";
        return false;
    }
    return true;
}

bool RecordFile::DumpStream()
{
    m_file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QDataStream dumpstream(&m_file);
    dumpstream << networinfo->cookies;
    dumpstream << networinfo->area;
    dumpstream << networinfo->url;
    dumpstream << networinfo->refresh_token;
    dumpstream << networinfo->user_id;
    m_file.close();
    return true;
}

bool RecordFile::LoadStream()
{
    networinfo = new NetWorkInfo();
    if (!m_file.open(QIODevice::ReadOnly)) {
        qDebug() << "文件不存在";
        return false;
    }
    QDataStream loadstream(&m_file);
    loadstream >> networinfo->cookies;
    loadstream >> networinfo->area;
    loadstream >> networinfo->url;
    loadstream >> networinfo->refresh_token;
    loadstream >> networinfo->user_id;
    m_file.close();
    return true;
}

QByteArray RecordFile::DumpNotRecord(QJsonObject obj)
{
    // bool tofile just used to
    QByteArray byte_array = QJsonDocument(obj).toJson(QJsonDocument::Indented);

    return byte_array;
}

QJsonObject RecordFile::LoadNotRecord(QByteArray text)
{
    QJsonObject jsonobj;
    if (text.size() <= 0) {
        qDebug() << "no text";
    }
    QJsonParseError json_err;
    QJsonDocument json_doc = QJsonDocument::fromJson(text, &json_err);
    if (json_err.error != QJsonParseError::NoError) {
        qDebug() << "Load json error: " << json_err.errorString();
    }

    if (json_doc.isObject()) {
        jsonobj = json_doc.object();
    } else {
        qDebug() << "Not JsonObject";
    }
    return jsonobj;
}
