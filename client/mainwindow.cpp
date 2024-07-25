#include "mainwindow.h"
#include "qrcodegen.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QPainter>
#include <QPixmap>
#include <QtNetwork/QNetworkRequest>
#include <iostream>
using namespace std;

using namespace qrcodegen;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GenQRcode()
{

    std::vector<QrSegment> segs = QrSegment::makeSegments(QString("你好，蠢朋\xF0\x9F\x90\xB7\xF0\x9F\x90\xB7\xF0\x9F\x90\xB7").toStdString().c_str());
    const QrCode qr = QrCode::encodeSegments(segs, QrCode::Ecc::MEDIUM, 7, 7);

    int width = ui->QRCode->width();
    int height = ui->QRCode->height();
    QPixmap pixmap(width, height);
    QPainter painter(&pixmap);
    painter.setBrush(Qt::black);
    double scale = width / qr.getSize();
    for (int x = 0; x < qr.getSize(); ++x) {
        for (int y = 0; y < qr.getSize(); ++y) {
            if (qr.getModule(x, y)) {
                QRect rect(x * scale, y * scale, scale, scale);
                painter.drawRect(rect);
            }
        }
    }

    ui->QRCode->setPixmap(pixmap);
}

void MainWindow::on_danmuButton_clicked()
{
    danmu.show();
}

void MainWindow::on_LoginButton_released()
{
    net = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setUrl(urls.QRrequestUrl);
    net->get(request);
    connect(net, &QNetworkAccessManager::finished, this, &MainWindow::slots_login_request_finished);
}

void MainWindow::on_testButton_clicked()
{
    GenQRcode();
}

void MainWindow::slots_login_request_finished(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QVariant statuscode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qDebug() << "登录请求错误: " << statuscode;
        return;
    }

    QByteArray data = reply->readAll();
    qDebug() << data;
    QJsonParseError jsonErr;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &jsonErr);
    if (jsonErr.error != QJsonParseError::NoError) {
        qDebug() << "无法解析服务器应答";
        return;
    }

    if (jsonDoc.isObject()) {
        const QJsonObject obj = jsonDoc.object();
        if (obj.contains("code")) {
            int code = obj.value("code").toInt(-1);
            if (code != 0) {
                qDebug() << "请求错误: " << code;
                return;
            }
        }
        if (obj.contains("data")) {
            QString url = obj.value("data")["url"].toString();
            QString qrcode_key = obj.value("data")["qrcode_key"].toString();
            qDebug() << "\n请求url: " << url << " " << "qrcode: " << qrcode_key;
        }
    }

    // 继续请求
}
