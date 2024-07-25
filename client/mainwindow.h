#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "danmuweight.h"
#include <QMainWindow>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public:
    void GenQRcode();
protected slots:
    void on_danmuButton_clicked();
    void on_LoginButton_released();
    void on_testButton_clicked();
    void slots_login_request_finished(QNetworkReply* reply);

private:
    Ui::MainWindow* ui;
    DanmuWeight danmu;
    QNetworkAccessManager* net;
    URLS urls;
};
#endif // MAINWINDOW_H
