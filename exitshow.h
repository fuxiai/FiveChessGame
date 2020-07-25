#ifndef EXITSHOW_H
#define EXITSHOW_H

#include <QDialog>
#include <QPainter>
#include <QPaintEvent>

namespace Ui {
class ExitShow;
}

class ExitShow : public QDialog
{
    Q_OBJECT

public:
    explicit ExitShow(QWidget *parent = nullptr);
    ~ExitShow();

private slots:
    void on_confirmBtn_clicked();
    void on_cancelBtn_clicked();
protected:
    void paintEvent(QPaintEvent *e);
private:
    Ui::ExitShow *ui;
};

#endif // EXITSHOW_H
