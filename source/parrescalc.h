#ifndef PARRESCALC_H
#define PARRESCALC_H

#include <QMainWindow>

namespace Ui {
    class ParResCalc;
}

class ParResCalc : public QMainWindow
{
    Q_OBJECT

public:
    explicit ParResCalc(QWidget *parent = 0);
    ~ParResCalc();

private:
    Ui::ParResCalc *ui;

private slots:
    void on_btnSearchR2_clicked();
    void on_btnSearchR1_clicked();
    void on_btnCalc_clicked();
};

#endif // PARRESCALC_H
