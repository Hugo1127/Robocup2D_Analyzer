#include "page.h"
#include "ui_page.h"
#include"mainwindow.h"
#include"autoplay/autoplay.h"
#include"analysis/analysis.h"
#include<QVBoxLayout>
Page::Page(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Page)
{
    ui->setupUi(this);
    //this->resize(800, 700); // 设置主窗口大小为
    setWindowTitle("Robocup2D"); // 替换为你的标题
    //实例化两个widget
    auto* p1 = new MainWindow;
    auto* p2 = new Autoplay;
    auto* p3 = new Analysis;
    //将两个widget添加至stackedWidget
    ui->stackedWidget->addWidget(p1);
    ui->stackedWidget->addWidget(p2);
    ui->stackedWidget->addWidget(p3);
    //设置stackedWidget当前展示页面
    ui->stackedWidget->setCurrentIndex(0);
    //连接两个页面上的按钮信号与相关响应操作
    connect(p1,&MainWindow::Autoplay_pressed,this,[=](){
        ui->stackedWidget->setCurrentIndex(1);
    });
    connect(p1,&MainWindow::Analysis_pressed,this,[=](){
        ui->stackedWidget->setCurrentIndex(2);
    });

    connect(p2,&Autoplay::mainwindow_pressed,this,[=](){
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(p3,&Analysis::to_mainwindow,this,[=](){
        ui->stackedWidget->setCurrentIndex(0);
    });
}

Page::~Page()
{
    delete ui;
}
