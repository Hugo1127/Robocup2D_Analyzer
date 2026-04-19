#ifndef DIALOG_H
#define DIALOG_H
#include <QDialog>
#include <QtNetwork>
#include <QUdpSocket>
QT_BEGIN_NAMESPACE
namespace Ui {
class Dialog;
}
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
private slots:

    void on_monitor_clicked();

    void on_send_clicked();

    void on_search_clicked();

    void on_close_clicked();

private:
    Ui::Dialog *ui; // UI指针
    QUdpSocket *udpsocket;
};
#endif // DIALOG_H
