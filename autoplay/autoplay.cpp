#include "autoplay.h"
#include "./ui_autoplay.h"
#include"server.h"
#include <QProcess>
#include<QTimer>
#include<QMessageBox>
#include <QFileDialog>
Autoplay::Autoplay(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Autoplay)
{
    ui->setupUi(this);
    setWindowTitle("Autoplay"); // 替换为你的标题
    _dialog=new Dialog(this);
}

Autoplay::~Autoplay()
{
    delete ui;
}

void Autoplay::on_start_clicked()
{
    if(startfile.size()<=1){
        QMessageBox::warning(this,
                             tr("警告"),
                             tr("no game！"),
                             QMessageBox::Ok);
        return;
    }
    // 遍历所有路径
    pairs={};
    for(int l = 0; l < startfile.size() - 1;l++) {//teamnum>=2
        QFileInfo fileInfo_l(startfile[l]);
        QString workingDir_l = fileInfo_l.absolutePath();  // 这会提取目录路径
        QString command_l = QString("cd '%1' && ./start.sh; exec bash").arg(workingDir_l);
        //QProcess::startDetached("x-terminal-emulator", {"-e", "bash", "-c", command_l});
        for(int r = l+1; r < startfile.size();r++) {
            QFileInfo fileInfo_r(startfile[r]);
            QString workingDir_r = fileInfo_r.absolutePath();  // 这会提取目录路径
            QString command_r = QString("cd '%1' && ./start.sh; exec bash").arg(workingDir_r);
            //QProcess::startDetached("x-terminal-emulator", {"-e", "bash", "-c", command_r});
            pairs.append(qMakePair(command_l, command_r));
        }
    }
    // 创建并启动线程
    QString serverpath = QFileDialog::getExistingDirectory(
        this,                       // 父窗口
        tr("选择比赛录像输出路径"),           // 对话框标题
        QDir::homePath(),// 标准路径
        QFileDialog::ShowDirsOnly   // 只显示目录
            | QFileDialog::DontResolveSymlinks
        );
    // 检查用户是否选择了有效路径
    if (serverpath.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("未选择有效路径！"), QMessageBox::Ok);
        return;
    }
    static int roundCount = 1; // 初始轮数
    QString roundFolderName;
    QString roundFolderPath;
    QDir dir;
    // 循环检查文件夹是否存在，若存在则递增轮数
    do {
        roundFolderName = QString("round_%1").arg(roundCount);
        roundFolderPath = serverpath + "/" + roundFolderName;
        roundCount++; // 先递增，避免后续重复判断
    } while (dir.exists(roundFolderPath));
    // 回退到正确的轮数值（因为循环退出时roundCount已多加了1）
    roundCount--;
    // 创建文件夹
    if (!dir.mkpath(roundFolderPath)) {
        QMessageBox::warning(this, tr("错误"), tr("无法创建文件夹！"), QMessageBox::Ok);
        return;
    }
    static ProcessManager *manager = new ProcessManager(roundFolderPath,pairs);
    //目前有bug，可能会连续开始
    if(manager->checkRunning()) {
        QMessageBox::warning(nullptr, "警告", "进程已在运行中！");
        return;
    }
    else
    manager->start();
    roundCount++;
    //delete manager;//程序会崩溃
}


void Autoplay::on_team1_clicked()//add team
{
    QString fileName = QFileDialog::getOpenFileName(
        this,                       // 父窗口
        tr("选择球队文件"),             // 对话框标题
        QDir::homePath(),           // 默认打开的目录
        tr("Start Files (*.sh)") // 文件过滤器
        );
    QFileDialog dialog(this);
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    if (!fileName.isEmpty()) {
        qDebug() << "选择的文件:" << fileName;
        QFile file(fileName);
        QTextStream in(&file);
        //1. 检查文件是否成功打开
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "打开文件失败:" << file.errorString(); // 明确错误原因
            return;
        }
        QString teamname="";
        QRegularExpressionMatch match;
        static QRegularExpression regex(//队名匹配
            R"(^teamname\s*=\s*(['"]?)(.*?)\1\s*$)",
            QRegularExpression::CaseInsensitiveOption); // 定义时一次性初始化
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("#")) continue; // 跳过注释行
            match = regex.match(line);
            if (match.hasMatch()) {
                teamname=match.captured(2);
                qDebug() << "teamname:" << teamname;
                file.close();
            }
        }
            if(!teamname.isEmpty()){
                // 获取或创建表头项
                QTableWidgetItem *horizonItem = ui->table_point->horizontalHeaderItem(team_num);
                if (!horizonItem) {
                    horizonItem = new QTableWidgetItem();
                    ui->table_point->setHorizontalHeaderItem(team_num, horizonItem);
                }
                QTableWidgetItem *verticalItem = ui->table_point->verticalHeaderItem(team_num);
                if (!verticalItem) {
                    verticalItem = new QTableWidgetItem();
                    ui->table_point->setVerticalHeaderItem(team_num, verticalItem);
                }
                horizonItem->setText(teamname);
                verticalItem->setText(teamname);
                startfile.append(fileName);
                // 检查是否需要扩展表格（仅在未填满时）
                if (team_num == ui->table_point->rowCount() - 1) {  // 改为检查是否到达最后一行
                    ui->table_point->insertRow(team_num + 1);  // 插入新行
                    ui->table_point->insertColumn(team_num+1);//插入列
                    // 设置新行和新列的表头名称
                    ui->table_point->setHorizontalHeaderItem(team_num + 1, new QTableWidgetItem("team"));
                    ui->table_point->setVerticalHeaderItem(team_num + 1, new QTableWidgetItem("team"));
                }
                    // 注意：这里不再自增 team_num，避免下次循环越界
                    team_num++;  // 仅在未触发插入时自增
            }
        }
    else return;
}


void Autoplay::on_team2_clicked()//delete team
{
    if(startfile.isEmpty()) {
        QMessageBox::warning(this,
                             tr("警告"),
                             tr("文件列表为空，请先添加文件！"),
                             QMessageBox::Ok);
        return;
    }
    // 检查当前行/列是否存在（避免越界）
    if (team_num - 1 >= 0 && team_num - 1 < ui->table_point->rowCount() &&
        team_num - 1 < ui->table_point->columnCount()) {
        // 删除行和列（从后往前删除，避免索引变化影响逻辑）
        ui->table_point->removeRow(team_num - 1);
        ui->table_point->removeColumn(team_num - 1);
    }
    if (team_num - 1 < startfile.size()) {
        startfile.removeAt(team_num-1);
        team_num--;  // 更新计数器
    }
    return;
}



void Autoplay::on_monitor_clicked()
{
    _dialog->show();

}


void Autoplay::on_point_clicked()
{
    // 检查路径是否为空
    if (roundFolderPath.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("比赛不存在！"), QMessageBox::Ok);
        return;
    }

}


void Autoplay::on_back_clicked()
{
    emit mainwindow_pressed();
}

