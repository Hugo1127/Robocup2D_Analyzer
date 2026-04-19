#ifndef AUTOPLAY_H
#define AUTOPLAY_H
#include"dialog.h"
#include <QMainWindow>
#include<QRegularExpression>
QT_BEGIN_NAMESPACE
namespace Ui {
class Autoplay;
}
QT_END_NAMESPACE

class Autoplay : public QMainWindow
{
    Q_OBJECT

public:
    Autoplay(QWidget *parent = nullptr);
    ~Autoplay();
    Dialog* _dialog;

signals:
    //按钮按下的处理函数中发出的信号
    void mainwindow_pressed();

private slots:
    void on_start_clicked();
    void on_team1_clicked();
    void on_team2_clicked();
    void on_monitor_clicked();
    void on_point_clicked();

    void on_back_clicked();

private:
    Ui::Autoplay *ui;
    int team_num=0;
    QString roundFolderPath;
    QStringList startfile;
    QList<QPair<QString, QString>> pairs;
    QString serverpath;
};
#endif // AUTOPLAY_H
