#ifndef PERIODICWIDGET_H
#define PERIODICWIDGET_H

#include <QWidget>

namespace Ui {
class PeriodicWidget;
}

class PeriodicWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PeriodicWidget(QWidget *parent = nullptr);
    ~PeriodicWidget();

private:
    Ui::PeriodicWidget *ui;
};

#endif // PERIODICWIDGET_H
