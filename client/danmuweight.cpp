#include "danmuweight.h"

#include "ui_danmuweight.h"

#include <QTimer>

#include "recordfile.h"
#include <QDateTime>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QIntValidator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <qstring.h>

DanmuWeight::DanmuWeight(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DanmuWeight)
    , m_width(291)
    , m_height(855)

{
    ui->setupUi(this);
    this->setWindowTitle("来聊天呀~\xF0\x9F\x98\x89");

    QIntValidator* validator1 = new QIntValidator(ui->danmuHeight);
    QIntValidator* validator2 = new QIntValidator(ui->danmuWidth);
    ui->danmuHeight->setValidator(validator1);
    ui->danmuWidth->setValidator(validator2);
    webView = new QWebEngineView(this);
    webView->setMinimumHeight(550);
    webView->setStyleSheet("margin-top:0px; margin-bottom:0px;");
    // webView->load(QUrl("https://chat.bilisc.com/room/DS4S72RKBI0W9?roomKeyType=2&showGiftName=true&mergeGift=false&showDebugMessages=true&lang=zh"));
    // webView->load(QUrl("https://www.baidu.com"));
    ui->verticalLayout->setSpacing(1);
    ui->verticalLayout->insertWidget(1, webView);

    connect(ui->MsgEdit, &QLineEdit::returnPressed, ui->SendMsgButton, &QPushButton::click);
}

DanmuWeight::~DanmuWeight()
{
    delete ui;
}

void DanmuWeight::slots_load_webengine(QUrl url, QList<std::pair<QString, QString>> cookie, QString room_id)
{
    m_cookie = cookie;
    m_room_id = room_id;
    webView->load(url);
}

void Timeout()
{
    qDebug() << "2 秒后";
}

void DanmuWeight::on_SendMsgButton_clicked()
{
    QNetworkAccessManager* net = new QNetworkAccessManager(this);
    QNetworkRequest request(m_url.SendDanmu);
    // 设置Cookie
    QByteArray cookieJar;
    QString cookie("");
    QString csrf;
    foreach (auto it, m_cookie) {
        cookie = cookie + it.first + "=" + it.second + ";";
        if (it.first == "bili_jct") {
            csrf = it.second;
        }
    }
    cookieJar.append(cookie.toUtf8());
    request.setRawHeader("Cookie", cookieJar);

    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch() / 1000);
    QUrlQuery postData;
    QString msg = ui->MsgEdit->text();
    msg.replace("+", "%2B");
    postData.addQueryItem("bubble", "0");
    postData.addQueryItem("msg", msg);
    postData.addQueryItem("color", "16777215");
    postData.addQueryItem("mode", "1");
    postData.addQueryItem("room_type", "0");
    postData.addQueryItem("fontsize", "25");
    postData.addQueryItem("roomid", m_room_id);
    postData.addQueryItem("rnd", timestamp);
    postData.addQueryItem("csrf", csrf);
    postData.addQueryItem("csrf_token", csrf);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    qDebug() << postData.toString(QUrl::FullyEncoded).toUtf8() << "________________";
    QNetworkReply* reply = net->post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
    // qDebug() << "---------------------------------";
    // qDebug() << csrf;
    // qDebug() << cookie;
    // qDebug() << timestamp;
    // qDebug() << "---------------------------------";
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "发送弹幕请求失败";
            return;
        }
        QJsonObject resp = RecordFile::LoadNotRecord(reply->readAll());
        qDebug() << resp;
        this->ui->MsgEdit->clear();
    });
}

void DanmuWeight::on_danmuRectBtn_clicked()
{
    if (ui->danmuWidth->text().isEmpty()) {
        m_width = 291;
    } else {
        m_width = ui->danmuWidth->text().toInt();
    }

    if (ui->danmuHeight->text().isEmpty()) {
        m_height = 885;
    } else {
        m_height = ui->danmuHeight->text().toInt();
    }

    this->resize(m_width, m_height);
}
