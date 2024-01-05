#ifndef DATESELECTOR_H
#define DATESELECTOR_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QCalendarWidget>

class DateSelector : public QWidget
{
    Q_OBJECT
public:
//    explicit DateSelector(QWidget *parent = nullptr);
    DateSelector(QWidget *parent = nullptr);
    QLineEdit *dateLineEdit;

signals:

private:

    QPushButton *selectButton;
    QCalendarWidget *calendarWidget;
};

#endif // DATESELECTOR_H
