#ifndef ATTACKWINDOW_H
#define ATTACKWINDOW_H

#include <QDialog>
#include <QMap>
#include <QFile>

namespace Ui {
class Attackwindow;
}

class Attackwindow : public QDialog
{
    Q_OBJECT

    QFile qf;
public:
    explicit Attackwindow(QWidget *parent = 0);
    ~Attackwindow();

private slots:
    void on_DataButton_pressed();

    void on_spinpts_start_valueChanged(int arg1);

    void on_spinpts_end_valueChanged(int arg1);

    void on_ClearButton_pressed();

    void on_attackButton_pressed();

private:
    Ui::Attackwindow *ui;
    QMap<QString, int> algo_map;
};

#endif // ATTACKWINDOW_H