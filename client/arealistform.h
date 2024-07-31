#ifndef AREALISTFORM_H
#define AREALISTFORM_H

#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QMap>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
namespace Ui {
class AreaListForm;
}

class AreaListForm : public QWidget {
    Q_OBJECT

public:
    explicit AreaListForm(QWidget* parent = nullptr);
    ~AreaListForm();

public:
    void AreaShow();
    QString m_area2id;

signals:
    void start_get_rtpm_code();

public slots:
    void slots_build_area(QJsonObject obj);

protected slots:
    void slots_change_area1(const QString& text);
    void slots_change_area2(const QString& text);
    void slots_close_button();

private:
    bool findArealist(QJsonValue v, QString& text);

private:
    Ui::AreaListForm* ui;
    // QTimer* m_timer;
    QString m_area1;
    QString m_area2;

    QJsonArray m_list_area1;
    QJsonArray m_list_area2;

    QVBoxLayout* m_layout;
    QLabel* m_areaLabel;
    QListWidget* m_area_1;
    QListWidget* m_area_2;

public:
    QJsonObject model;
};

#endif // AREALISTFORM_H
