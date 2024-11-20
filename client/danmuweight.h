#ifndef DANMUWEIGHT_H
#define DANMUWEIGHT_H

#include "publiclib.h"
#include <QBrush>
#include <QCloseEvent>
#include <QLabel>
#include <QList>
#include <QListWidget>
#include <QUrl>
#include <QWidget>
#include <QtWebEngineWidgets>
class Message;

namespace Ui {
class DanmuWeight;
}

class DanmuWeight : public QWidget {
    Q_OBJECT

public:
    explicit DanmuWeight(QWidget* parent = nullptr);
    ~DanmuWeight();

public:
    void msgShow();
public slots:
    void slots_load_webengine(QUrl url, QList<std::pair<QString, QString>>, QString room_id);
protected slots:
    void on_SendMsgButton_clicked();
    void on_danmuRectBtn_clicked();

protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::DanmuWeight* ui;

private:
    qint16 m_width;
    qint16 m_height;
    QWebEngineView* webView;
    QList<std::pair<QString, QString>> m_cookie;
    URLS m_url;
    QString m_room_id;
};

#endif // DANMUWEIGHT_H
