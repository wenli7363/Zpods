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

signals:

private:
    QLineEdit *dateLineEdit;
    QPushButton *selectButton;
    QCalendarWidget *calendarWidget;
};

#endif // DATESELECTOR_H
