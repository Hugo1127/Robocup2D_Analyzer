#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<QMainWindow>
// #include"autoplay/autoplay.h"
// #include"analysis/analysis.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //Autoplay* _autoplay;
signals:
    //按钮按下的处理函数中发出的信号
    void Autoplay_pressed();
    void Analysis_pressed();
private slots:
    void on_Autoplay_clicked();

    void on_Analysis_clicked();

private:
    Ui::MainWindow *ui; // UI指针
};
#endif // MAINWINDOW_H
