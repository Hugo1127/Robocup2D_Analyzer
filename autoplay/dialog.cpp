#include "dialog.h"
#include "./ui_dialog.h"
#include<QProcess>
#include<QMessageBox>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog) // 创建UI实例
{
    ui->setupUi(this); // 初始化UI布局
    setWindowTitle("Connect"); // 替换为你的标题
    //方式1：使用Qt5的新式连接语法（推荐）
    // connect(ui->monitor, &QPushButton::clicked,
    //         this, &Dialog::on_monitor_clicked);
    udpsocket = new QUdpSocket(this);
    ui->portEdit->setText("2025");//默认端口号
    QList<QHostAddress> ipList = QNetworkInterface::allAddresses();
    //for (const auto&ipItem : ipList) {
    foreach(QHostAddress ipItem, ipList){
        //只显示以192开头的IP地址
        if(ipItem.protocol()==QAbstractSocket::IPv4Protocol&&ipItem!=QHostAddress::Null
            &&ipItem!=QHostAddress::LocalHost&&ipItem.toString().left(3)=="192"){
            ui->myipedit->setText(ipItem.toString());
        }
    }
}
Dialog::~Dialog()
{
    delete ui; // 释放UI资源
}
void Dialog::on_monitor_clicked()
{
    QProcess process;
    process.startDetached("x-terminal-emulator", {"-e", "bash", "-c", "rcssmonitor; exec bash"});
}

void Dialog::on_send_clicked()//udp广播自己的ip，然后等待接受
{
    udpsocket->open(QIODevice::ReadWrite);
    // 将字符串转换为字节数组
    QByteArray data = ui->myipedit->text().toUtf8();
    // 定义广播地址和端口
    QHostAddress broadcastAddress = QHostAddress::Broadcast; // 广播地址
    quint16 port = ui->portEdit->text().toUInt();
    // 发送广播数据
    qint64 bytesSent = udpsocket->writeDatagram(data, broadcastAddress, port);
    // 检查是否发送成功
    if (bytesSent == -1) {
        QMessageBox::critical(this, "广播失败", "发送失败：" + udpsocket->errorString());
    } else {
        QMessageBox::information(this, "广播成功", "广播成功，字节数：" + QString::number(bytesSent));
    }
}
void Dialog::on_search_clicked()//监听局域网内的广播，如果接受到，获取server的ip，在终端输入指令连接到server
{
    udpsocket->open(QIODevice::ReadWrite);
    // 2. 绑定到本地端口
    if (!udpsocket->bind(ui->portEdit->text().toUInt(), QUdpSocket::ShareAddress)) {
        QMessageBox::critical(this, "绑定失败", "无法绑定到端口：" + udpsocket->errorString());
        return;
    }
    QMessageBox::information(this, "等待server连接", "请勿频繁点击");
    connect(udpsocket, &QUdpSocket::readyRead, this, [=](){
        while(udpsocket->hasPendingDatagrams())
        {
            QByteArray data;
            data.resize(udpsocket->pendingDatagramSize());
            QHostAddress host;
            quint16 port;
            udpsocket->readDatagram(data.data(), data.size(), &host, &port);
            QString text = QString("[%1:%2]:%3").arg(host.toString()).arg(port).arg(QString(data));
            QStringList parts = text.split(':', Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                QString message = parts.last();  // 提取发送内容
                qDebug() << "Extracted message:" << message;
                // 添加弹窗提示
                QMessageBox::information(this, "收到消息", QString("来自 %1:%2 的消息: %3").arg(host.toString()).arg(port).arg(message));
                QProcess process;
                process.startDetached("x-terminal-emulator", {"-e", "bash", "-c", "rcssmonitor --server-host "+message+"; exec bash"});
            } else {
                qDebug() << "Invalid format";
            }
        }
    });
}


void Dialog::on_close_clicked()
{
    udpsocket->close();
    QMessageBox::information(this, "等待server连接", "已停止等待");
}

