#include "mainwindow.h"
#include "./ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
, ui(new Ui::MainWindow){
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui; // 释放UI资源
}

void MainWindow::on_Autoplay_clicked()
{
emit Autoplay_pressed();
}


void MainWindow::on_Analysis_clicked()
{
emit Analysis_pressed();
}

