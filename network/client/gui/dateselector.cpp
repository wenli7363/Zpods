#include "dateselector.h"
#include <QLayout>

DateSelector::DateSelector(QWidget *parent) : QWidget(parent) {
    QHBoxLayout *layout = new QHBoxLayout(this);
    dateLineEdit = new QLineEdit(this);
    selectButton = new QPushButton("选择日期", this);

    layout->addWidget(dateLineEdit);
    layout->addWidget(selectButton);

    calendarWidget = new QCalendarWidget(this);
    calendarWidget->setWindowFlags(Qt::Popup);
    calendarWidget->setWindowModality(Qt::WindowModal);
    calendarWidget->setSelectedDate(QDate::currentDate());
    calendarWidget->setDateEditEnabled(true);

    connect(selectButton, &QPushButton::clicked, this, [this](){
        QPoint pos = selectButton->mapToGlobal(QPoint(0, selectButton->height()));
        calendarWidget->move(pos);
        calendarWidget->show();
    });


    connect(calendarWidget, &QCalendarWidget::selectionChanged, this, [this](){
        QDate selectedDate = calendarWidget->selectedDate();
        dateLineEdit->setText(selectedDate.toString("yyyy-MM-dd"));
        calendarWidget->hide();
    });
}


