#include "danmuweight.h"
#include "ui_danmuweight.h"

#include <QTimer>
#include <qstring.h>

DanmuWeight::DanmuWeight(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DanmuWeight)
    , msgItem(nullptr)
{
    ui->setupUi(this);
    connect(ui->MsgEdit, &QLineEdit::returnPressed, ui->SendMsgButton, &QPushButton::released);
}

DanmuWeight::~DanmuWeight()
{
    delete ui;
    for (auto i : msgs) {
        delete i;
    }
}

void Timeout()
{
    qDebug() << "2 秒后";
}

void DanmuWeight::on_SendMsgButton_released()
{
    QString msg = ui->MsgEdit->text();
    msgItem = new Message(msg);
    msgItem->mitem->setSizeHint(QSize(0, 30));
    ui->MsgText->addItem(msgItem->mitem);
    msgs.append(msgItem);
    ui->MsgEdit->clear();
}
