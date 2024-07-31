#include "mainwindow.h"

#include "libs/qrcodegen.h"

#include "ui_mainwindow.h"
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QPainter>
#include <QPixmap>
#include <QUrlQuery>
#include <QVariantMap>
#include <QtNetwork/QNetworkRequest>
#include <iostream>

using namespace std;

using namespace qrcodegen;

enum LoginStatus {
    SUCCESSED = 0,
    UNSCANNED = 86101,
    QREXPIRED = 86038,
    QRNOTCONFIRMED = 86090
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_timer_login_status(nullptr)
    , m_networkinfo()
    , file(new RecordFile("./bili-cookie.bili"))
{
    ui->setupUi(this);
    this->setWindowTitle("bilibili-mac");
    connect(this, &MainWindow::area_form_show, &arealist_form, &AreaListForm::slots_build_area);
    connect(this, &MainWindow::load_damu_webengine, &danmu, &DanmuWeight::slots_load_webengine);
    connect(&arealist_form, &AreaListForm::start_get_rtpm_code, this, &MainWindow::slots_start_get_rtmp_code);

    file->LoadStream();
    m_networkinfo = *file->networinfo;
    ui->bilivechatLine->setText(m_networkinfo.blivechat_url);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (m_timer_login_status != nullptr) {

        delete m_timer_login_status;
    }
}

void MainWindow::GenQRcode(QString url)
{

    std::vector<QrSegment> segs = QrSegment::makeSegments(url.toStdString().c_str());
    const QrCode qr = QrCode::encodeSegments(segs, QrCode::Ecc::MEDIUM, 7, 7);

    int width = ui->QRCode->width();
    int height = ui->QRCode->height();
    QPixmap pixmap(width, height);
    QPainter painter(&pixmap);
    painter.setBrush(Qt::black);
    int scale = width / qr.getSize();
    double offset = (width - scale * qr.getSize()) / 2;
    for (int x = 0; x < qr.getSize(); ++x) {
        for (int y = 0; y < qr.getSize(); ++y) {
            if (qr.getModule(x, y)) {
                QRect rect(x * scale + offset, y * scale + offset, scale, scale);
                painter.drawRect(rect);
            }
        }
    }

    ui->QRCode->setPixmap(pixmap);
}

void MainWindow::CheckLogin()
{
}

void MainWindow::DeleteTimer(QTimer* timer)
{
    timer->stop();
    delete timer;
    timer = nullptr;
}

void MainWindow::on_danmuButton_clicked()
{
    m_networkinfo.blivechat_url = ui->bilivechatLine->toPlainText().trimmed();
    file->networinfo->blivechat_url = m_networkinfo.blivechat_url;
    file->DumpStream();

    QEventLoop loop;
    QNetworkAccessManager* net = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setUrl(m_urls.GetRoomId + m_networkinfo.user_id);

    QNetworkReply* reply = net->get(request);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QByteArray r = reply->readAll();
    reply->deleteLater();

    QJsonObject data = RecordFile::LoadNotRecord(r);

    QString room_id = data.toVariantMap()["data"].toMap()["room_id"].toString();

    emit load_damu_webengine(QUrl(m_networkinfo.blivechat_url), m_networkinfo.cookies, room_id);
    danmu.show();
}

void MainWindow::on_LoginButton_released()
{
    net = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setUrl(m_urls.QRrequestUrl);
    net->get(request);
    connect(net, &QNetworkAccessManager::finished, this, &MainWindow::slots_login_request_finished);
}

void MainWindow::on_StartLiveButton_clicked()
{
    QNetworkAccessManager* net = new QNetworkAccessManager(this);
    QNetworkRequest req;
    req.setUrl(m_urls.AreaListUrl);
    QNetworkReply* reply = net->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            QVariant statuscode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            qDebug() << "获取播放区列表错误: " << statuscode;
            return;
        }
        m_area_obj = RecordFile::LoadNotRecord(reply->readAll());
        arealist_form.show();

        emit area_form_show(m_area_obj);
    });
}

void MainWindow::on_testButton_clicked()
{

    file->LoadStream();
    QList cookie = file->networinfo->cookies.toList();
    QString s;
    for (auto it = cookie.begin(); it != cookie.end(); ++it) {
        if (it->second != "") {
            s = it->first + "=" + it->second + ";";
            qDebug() << s;
        }
    }

    qDebug() << s;
}

void MainWindow::slots_login_request_finished(QNetworkReply* reply)
{

    if (reply->error() != QNetworkReply::NoError) {
        QVariant statuscode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qDebug() << "登录请求错误: " << statuscode;
        return;
    }

    QByteArray data = reply->readAll();

    QJsonObject json_obj = RecordFile::LoadNotRecord(data);
    qDebug() << "json size" << json_obj.size();
    if (json_obj.size() <= 0) {
        qDebug() << "无法解析服务器应答";
        return;
    }

    if (json_obj.contains("code")) {
        int code = json_obj.value("code").toInt(-1);
        if (code != 0) {
            qDebug() << "请求错误: " << code;
            return;
        }
    }

    if (json_obj.contains("data")) {
        QString url = json_obj.value("data")["url"].toString();
        m_urls.SetLoginUrl(json_obj.value("data")["qrcode_key"].toString());
        qDebug() << "\n请求url: " << m_urls.CheckLoginUrl;
        GenQRcode(url);
    }

    if (m_timer_login_status == nullptr) {
        m_timer_login_status = new QTimer(this);
        connect(m_timer_login_status, &QTimer::timeout, this, &MainWindow::slots_check_login_status);
        m_currtime = QDateTime::currentDateTime();
        m_timer_login_status->start(3000);
    }
}

void MainWindow::slots_check_login_status()
{
    QDateTime currtime = QDateTime::currentDateTime();
    qint64 elapsed_seconds = m_currtime.secsTo(currtime);

    if (elapsed_seconds >= 120) {
        qDebug() << "二维码失效：timer delete ->" << m_timer_login_status;
        ui->textBrowser->setText("二维码失效");
        DeleteTimer(m_timer_login_status);
    } else {
        QNetworkAccessManager* net = new QNetworkAccessManager(this);
        QNetworkRequest request;
        qDebug() << "request check login url: " << m_urls.CheckLoginUrl;
        request.setUrl(m_urls.CheckLoginUrl);
        QNetworkReply* reply = net->get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() != QNetworkReply::NoError) {
                QVariant statuscode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
                qDebug() << "登录状态检查错误: " << statuscode;
                return;
            }
            QByteArray data = reply->readAll();
            // testtest
            QList<QByteArray> headers = reply->rawHeaderList().toList();
            qDebug() << "--------------------------------";
            foreach (auto it, headers) {
                if (it.toStdString() == "set-cookie") {
                    qDebug() << reply->rawHeader(it).toStdString();
                }
            }
            qDebug() << "--------------------------------";
            // testtest
            this->ui->textBrowser->setText("检查登录状态");
            this->m_check_login_status(data);
        });

        qDebug() << "timer~ " << elapsed_seconds;
    }
}

void MainWindow::slots_start_get_rtmp_code()
{

    m_networkinfo = *file->networinfo;

    // 每次需要新获取的
    QEventLoop loop;
    QNetworkAccessManager* net = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setUrl(m_urls.GetRoomId + m_networkinfo.user_id);
    // qDebug() << m_urls.GetRoomId + m_networkinfo.user_id;
    QNetworkReply* reply = net->get(request);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    // TODO: 异常处理，保存area
    QByteArray r = reply->readAll();
    reply->deleteLater();

    QJsonObject data = RecordFile::LoadNotRecord(r);
    // qDebug() << data;

    QString room_id = data.toVariantMap()["data"].toMap()["room_id"].toString();

    request.setUrl(m_urls.StartLiveUrl);

    // 设置请求头的Content-Type
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    // 设置COOKIE
    QList<QNetworkCookie> cookies;
    foreach (auto it, m_networkinfo.cookies) {
        if ("SESSDATA" == it.first) {
            cookies.append(QNetworkCookie(it.first.toUtf8(), it.second.toUtf8()));
        } else if ("bili_jct" == it.first) {
            cookies.append(QNetworkCookie(it.first.toUtf8(), it.second.toUtf8()));
        }
    }
    QNetworkCookieJar* cookjar = net->cookieJar();
    cookjar->setCookiesFromUrl(cookies, m_urls.StartLiveUrl);

    // 设置POST数据
    QUrlQuery post_data;
    foreach (auto it, m_networkinfo.cookies) {
        if ("bili_jct" == it.first) {
            post_data.addQueryItem("csrf", it.second);
        }
    }

    post_data.addQueryItem("room_id", room_id);
    post_data.addQueryItem("platform", "web_link");
    post_data.addQueryItem("area_v2", arealist_form.m_area2id);

    QNetworkReply* reply_rtmp = net->post(request, post_data.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply_rtmp, &QNetworkReply::finished, this, [this, reply_rtmp]() {
        if (reply_rtmp->error() != QNetworkReply::NoError) {
            qDebug() << "请求rtmp错误";
            return;
        }

        QJsonObject rtmpdata = RecordFile::LoadNotRecord(reply_rtmp->readAll());
        qDebug() << rtmpdata;
        QString rtmp_url = rtmpdata.toVariantMap()["data"].toMap()["rtmp"].toMap()["addr"].toString();

        QString rtmp_code = rtmpdata.toVariantMap()["data"].toMap()["rtmp"].toMap()["code"].toString();
        this->ui->textBrowser->setText(rtmp_url + "\n" + rtmp_code);
    });
}

void MainWindow::on_StopLiveButton_clicked()
{
    file->LoadStream();
    m_networkinfo = *file->networinfo;
    QEventLoop loop;
    QNetworkAccessManager* net = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setUrl(m_urls.GetRoomId + m_networkinfo.user_id);

    QNetworkReply* reply = net->get(request);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    // TODO: 异常处理，保存area，保存room_id
    QByteArray r = reply->readAll();
    reply->deleteLater();

    QJsonObject data = RecordFile::LoadNotRecord(r);

    QString room_id = data.toVariantMap()["data"].toMap()["room_id"].toString();
    request.setUrl(m_urls.StopLiveUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QString csrf;

    // 设置cookie
    QList<QNetworkCookie> cookies;
    foreach (auto it, m_networkinfo.cookies) {
        if (it.first == "bili_jct") {
            cookies.append(QNetworkCookie(it.first.toUtf8(), it.second.toUtf8()));
            csrf = it.second;
        } else if (it.first == "SESSDATA") {
            cookies.append(QNetworkCookie(it.first.toUtf8(), it.second.toUtf8()));
        }
    }
    QNetworkCookieJar* cookiejar = net->cookieJar();
    cookiejar->setCookiesFromUrl(cookies, m_urls.StopLiveUrl);

    QUrlQuery post_data;
    post_data.addQueryItem("csrf", csrf);
    post_data.addQueryItem("room_id", room_id);
    qDebug() << csrf << room_id;
    reply = net->post(request, post_data.toString(QUrl::FullyEncoded).toUtf8());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "停播失败" << reply->error();
            return;
        }

        QJsonObject obj = RecordFile::LoadNotRecord(reply->readAll());
        qDebug() << obj;
        QString text = "回复状态：\n";
        text = text + "code: " + obj.toVariantMap()["code"].toString() + "\n";
        text = text + "change: " + obj.toVariantMap()["data"].toMap()["change"].toString() + "\n";
        text = text + "status: " + obj.toVariantMap()["data"].toMap()["status"].toString() + "\n";
        if (obj.toVariantMap()["data"].toMap()["status"].toString() == "PREPARING") {
            text = text + "已停播";
        }
        this->ui->textBrowser->setText(text);
    });
}

void MainWindow::m_check_login_status(QByteArray& data)
{

    QJsonObject json_obj = RecordFile::LoadNotRecord(data);
    if (json_obj.size() <= 0) {
        qDebug() << "无法解析服务器应答";
        ui->textBrowser->setText("无法解析服务器应答");
    }

    QJsonObject respdata = json_obj["data"].toObject();
    qint64 respdata_code = respdata["code"].toInt(-1);
    qDebug() << respdata;

    switch (respdata_code) {
    case SUCCESSED: {
        DeleteTimer(m_timer_login_status);
        m_networkinfo.refresh_token = respdata["refresh_token"].toString();
        m_networkinfo.url = QUrl(respdata["url"].toString());

        QUrlQuery query(m_networkinfo.url);
        m_networkinfo.cookies = query.queryItems();
        foreach (auto it, m_networkinfo.cookies) {
            qDebug() << it.first << it.second;
            if (it.first == "DedeUserID") {
                m_networkinfo.user_id = it.second;
                break;
            }
        }
        file->networinfo = &m_networkinfo;
        file->DumpStream();
        ui->textBrowser->setText("登录成功");
        // qDebug() << "---------login status----------";
        // qDebug() << m_networkinfo.refresh_token;
        // qDebug() << m_networkinfo.url;
        // qDebug() << m_networkinfo.cookies;
        // qDebug() << m_networkinfo.area;
        // qDebug() << "---------------------------";
        return;
    }
    case UNSCANNED:
        qDebug() << "请扫码手机确认登录";
        ui->textBrowser->setText("请扫码手机确认登录");
        break;

    case QREXPIRED:
        qDebug() << "二维码已失效 点击重新登录 timer delete ->" << m_timer_login_status;
        ui->textBrowser->setText("二维码已失效 点击登录按钮重新登录");
        DeleteTimer(m_timer_login_status);

        break;

    case QRNOTCONFIRMED:
        qDebug() << "请尽快在手机确认登录";
        ui->textBrowser->setText("请尽快在手机上确认登录");
        break;
    default:
        qDebug() << "emmm~ 接口改了？";
        ui->textBrowser->setText("emmm...接口改了？");
        break;
    }
}
