#ifndef DANMUWEIGHT_H
#define DANMUWEIGHT_H

#include "publiclib.h"
#include <QBrush>
#include <QLabel>
#include <QListWidget>
#include <QWidget>

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
protected slots:
    void on_SendMsgButton_released();

private:
    Ui::DanmuWeight* ui;
    Message* msgItem;
    QVector<Message*> msgs;
};

class Message {
public:
    Message(QString& msg)
    {
        mitem = new QListWidgetItem(msg);
        QLinearGradient gradient(QPointF(0, 0), QPointF(0, 30));
        gradient.setColorAt(0, QColor::fromRgb(255, 255, 224)); // 浅黄色
        gradient.setColorAt(1, QColor::fromRgb(255, 165, 0)); // 黄色
        QBrush gradientBrush(gradient);
        mitem->setBackground(gradientBrush);
    }
    ~Message()
    {
        delete mitem;
    }

public:
    QListWidgetItem* mitem;
};
#endif // DANMUWEIGHT_H
