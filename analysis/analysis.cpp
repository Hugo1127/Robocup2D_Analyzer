#include "analysis.h"
#include "ui_analysis.h"
#include"rcg2csv.h"
#include <QFileDialog>
#include <QMessageBox>
Analysis::Analysis(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Analysis)
{
    ui->setupUi(this);
    ui->output->setText(QDir::homePath());
    ui->cycle->setText("10");
}

Analysis::~Analysis()
{
    delete ui;
}

void Analysis::on_back_clicked()
{
    emit to_mainwindow();
}

void Analysis::on_start_clicked()
{
    if (rcg_path.empty()) {
        QMessageBox::warning(this, tr("提示"), tr("路径不能为空！"));
        return;
    }
    QString s1=ui->cycle->text();
    int cycle=s1.toInt();
    if (cycle<1||cycle>5999){
        QMessageBox::warning(this, tr("提示"), tr("不合法的周期！（1到6000）"));
        return;
    }
    std::string output_path=ui->output->text().toStdString();
    QThread *Thread=new QThread();
    Logprocess *log = new Logprocess(nullptr,rcg_path,cycle,output_path);  // 必须无父对象
    log->moveToThread(Thread);    // 移动到子线程
    // 3. 连接信号槽
    QObject::connect(Thread, &QThread::started, log, &Logprocess::dowork);
    QObject::connect(log, &Logprocess::work_finished, Thread, &QThread::quit);
    QObject::connect(log, &Logprocess::work_finished, log, &Logprocess::deleteLater);
    QObject::connect(Thread, &QThread::finished, Thread, &QThread::deleteLater);
    //4. 启动线程 m
    Thread->start();
    QMessageBox::information(this, tr("提示"), tr("解析成功！"));
    return;
}
void Analysis::on_input_choose_clicked()
{
    // 打开文件对话框，限制只能选取后缀为 .rcg
    QString filter = "Rcg or Rcl Files (*.rcg )";
    QString fileName = QFileDialog::getOpenFileName(this, "选择文件", QDir::homePath(), filter);
    // 将选中的文件路径显示到 textEdit 上
    ui->input->setText(fileName);
    rcg_path=fileName.toStdString();
    qDebug()<<rcg_path;
    // 获取所选rcg文件信息
    QFileInfo rcgFileInfo(fileName);
    QString rcgDirPath = rcgFileInfo.absolutePath();   // 文件所在目录
    QString baseName = rcgFileInfo.completeBaseName(); // 不含后缀的文件名
    // 构造对应的.rcl文件路径
    QString rclFilePath = rcgDirPath + QDir::separator() + baseName + ".rcl";
    // 判断对应的rcl文件是否存在
    if (!QFileInfo::exists(rclFilePath))
    {
        QMessageBox::warning(this, "警告", QString("对应的 RCL 文件不存在:\n%1").arg(rclFilePath));
        // 如果需要，也可以把textEdit中rcl路径清空或者做其它处理
        ui->input->clear();
    }
}
void Analysis::on_output_choose_clicked()
{
    output_path = QFileDialog::getExistingDirectory(
        this,                       // 父窗口
        tr("选择csv输出路径"),           // 对话框标题
        QDir::homePath(),// 标准路径
        QFileDialog::ShowDirsOnly   // 只显示目录
            | QFileDialog::DontResolveSymlinks
        );
    ui->output->setText(output_path);
}

