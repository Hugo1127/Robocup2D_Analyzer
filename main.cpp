#include "page.h"
#include <QApplication>
#include <QFile>
#include<QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile qssFile(":/design.qss");//1.
    if(qssFile.open(QFile::ReadOnly)){//2.
        //qDebug()<<"1";
        a.setStyleSheet(qssFile.readAll());//3.
    }
    qssFile.close();//4.
    Page w;
    w.show();
    return a.exec();
}
