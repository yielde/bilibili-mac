#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "arealistform.h"
#include "danmuweight.h"
#include "mainwindow.h"
#include "publiclib.h"
#include "recordfile.h"
#include <QDateTime>
#include <QList>
#include <QMainWindow>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class RecordFile;
class NetWorkInfo : public QObject {
public:
    NetWorkInfo()
        : cookies()
        , refresh_token("")
        , url(QUrl(""))
        , area("")
        , user_id("")
        , blivechat_url("")

    {
    }
    NetWorkInfo(const NetWorkInfo& info)
    {
        cookies = info.cookies;
        refresh_token = info.refresh_token;
        url = info.url;
        area = info.area;
        user_id = info.user_id;
        blivechat_url = info.blivechat_url;
    }
    NetWorkInfo& operator=(const NetWorkInfo& info)
    {
        if (this != &info) {
            cookies = info.cookies;
            refresh_token = info.refresh_token;
            url = info.url;
            area = info.area;
            user_id = info.user_id;
            blivechat_url = info.blivechat_url;
        }
        return *this;
    }

public:
    QList<std::pair<QString, QString>> cookies;
    QString refresh_token;
    QUrl url;
    QString area;
    QString user_id;
    QString blivechat_url;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public:
    void GenQRcode(QString url);
    void CheckLogin();
    void DeleteTimer(QTimer* timer);
protected slots:
    void on_danmuButton_clicked();
    void on_LoginButton_released();
    void on_StartLiveButton_clicked();
    void on_StopLiveButton_clicked();

    void slots_login_request_finished(QNetworkReply* reply);
    void slots_check_login_status();
    void slots_start_get_rtmp_code();
    void on_testButton_clicked();

signals:
    void area_form_show(QJsonObject obj);
    void load_damu_webengine(QUrl url, QList<std::pair<QString, QString>> cookie, QString room_id);

private:
    void m_check_login_status(QByteArray& data);

private:
    Ui::MainWindow* ui;
    DanmuWeight danmu;
    AreaListForm arealist_form;
    QNetworkAccessManager* net;
    URLS m_urls;
    QTimer* m_timer_login_status;
    QJsonObject m_area_obj;
    QDateTime m_currtime;
    NetWorkInfo m_networkinfo;
    RecordFile* file;
};

#endif // MAINWINDOW_H
