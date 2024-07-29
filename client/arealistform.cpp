#include "arealistform.h"
#include "ui_arealistform.h"

#include <QVariant>
#include <algorithm>

AreaListForm::AreaListForm(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AreaListForm)
    , model()
{
    ui->setupUi(this);
    // m_timer = new QTimer(this);
    // connect(m_timer, &QTimer::timeout, this, &AreaListForm::AreaShow);
    // m_timer->start(3000);

    m_layout = new QVBoxLayout(this);

    m_areaLabel = new QLabel("选择一个区域", this);
    m_layout->addWidget(m_areaLabel);

    m_area_1 = new QListWidget(this);
    m_layout->addWidget(m_area_1);

    m_area_2 = new QListWidget(this);
    m_layout->addWidget(m_area_2);

    QPushButton* close_btn = new QPushButton(this);
    close_btn->setText("确定");
    m_layout->addWidget(close_btn);
    connect(close_btn, &QPushButton::clicked, this, &AreaListForm::slots_close_button);
    connect(m_area_1, &QListWidget::currentTextChanged, this, &AreaListForm::slots_change_area1);
    connect(m_area_2, &QListWidget::currentTextChanged, this, &AreaListForm::slots_change_area2);
}

AreaListForm::~AreaListForm()
{
    delete ui;
}

void AreaListForm::AreaShow()
{
}

void AreaListForm::slots_build_area(QJsonObject obj)
{

    if (obj["code"] == 0) {
        m_list_area1 = obj["data"].toArray();
        for (auto it = m_list_area1.begin(); it != m_list_area1.end(); ++it) {
            m_area_1->addItem(it->toObject()["name"].toString());
        }
    }
}

void AreaListForm::slots_change_area1(const QString& text)
{
    auto it = std::find_if(m_list_area1.begin(), m_list_area1.end(), std::bind(&AreaListForm::findArealist, this, std::placeholders::_1, text));

    m_area1 = it->toObject()["name"].toString();

    m_areaLabel->setText(m_area1 + ":");
    m_list_area2 = it->toObject()["list"].toArray();
    m_area_2->clear();
    for (auto j = m_list_area2.begin(); j != m_list_area2.end(); ++j) {
        this->m_area_2->addItem(j->toObject()["name"].toString());
    }
}

void AreaListForm::slots_change_area2(const QString& text)
{
    auto it = std::find_if(m_list_area2.begin(), m_list_area2.end(), std::bind(&AreaListForm::findArealist, this, std::placeholders::_1, text));
    QJsonObject obj = it->toObject();
    m_area2 = obj["name"].toString();
    m_areaLabel->setText(m_area1 + ":" + m_area2);
    m_area2id = obj["id"].toString();
}

void AreaListForm::slots_close_button()
{
    emit start_get_rtpm_code();
    this->close();
}

bool AreaListForm::findArealist(QJsonValue v, QString& text)
{
    return v.toObject()["name"].toString() == text;
}

// void AreaListForm::build_area2(QJsonArray arr, const QString& text)
// {
//     auto it = std::find_if(arr.begin(), arr.end(), std::bind(&AreaListForm::findArealist, this, std::placeholders::_1, text));

//     m_area1 = it->toObject()["name"].toString();

//     m_areaLabel->setText(m_area1 + ":");
//     QJsonArray list_area2 = it->toObject()["list"].toArray();
//     this->m_area_2->clear();
//     foreach (auto j, list_area2) {
//         this->m_area_2->addItem(j.toObject()["name"].toString());
//     }

//     connect(m_area_2, &QListWidget::currentTextChanged, this, [list_area2, this](const QString& text) {
//         auto it = std::find_if(list_area2.begin(), list_area2.end(), std::bind(&AreaListForm::findArealist, this, std::placeholders::_1, text));
//         m_area2 = it->toObject()["name"].toString();

//         m_areaLabel->setText(m_area1 + ":" + m_area2);
//         qDebug() << m_area1 << m_area2;
//     });
// }
