#ifndef SERVER_CPP
#define SERVER_CPP
#include"server.h"
#include<QDebug>
ProcessManager::ProcessManager(const QString serverpath, const QList<QPair<QString, QString>>& teamPairs, QObject *parent)
    : QThread(parent), m_teamPairs(teamPairs),_serverpath(serverpath)
{
    isRunning=false;
}

ProcessManager::~ProcessManager()
{
    quit();
    wait();
}

void ProcessManager::run()
{
    QProcess *serverProcess = nullptr;
    QProcess *team1Process = nullptr;
    QProcess *team2Process = nullptr;
    isRunning=true;
    for (int i = 0; i < m_teamPairs.size(); i++) {
        // 释放旧资源
        delete serverProcess;
        delete team1Process;
        delete team2Process;

        // 创建新进程
        serverProcess = new QProcess();
        team1Process = new QProcess();
        team2Process = new QProcess();

        // 启动服务器
        //serverProcess->start("x-terminal-emulator", {"-e", "bash", "-c", "rcssserver; exec bash"});
        serverProcess->start("x-terminal-emulator",
        {"-e", "bash", "-c", "cd " + _serverpath + " && rcssserver; exec bash"});
        sleep(1);
        qDebug() << "team_pairs:" << m_teamPairs[i].first << m_teamPairs[i].second;
        //team1Process->start("x-terminal-emulator", {"-e", "bash", "-c", m_teamPairs[i].first + "; exec bash"});
        //team2Process->start("x-terminal-emulator", {"-e", "bash", "-c", m_teamPairs[i].second + "; exec bash"});
        team1Process->start("bash", {"-c", m_teamPairs[i].first});
        team2Process->start("bash", {"-c", m_teamPairs[i].second});
        serverProcess->waitForFinished(-1); // 无限等待直到服务器结束
        qDebug() << "server down";
        team1Process->terminate();
        team2Process->terminate();
    }
    delete serverProcess;
    delete team1Process;
    delete team2Process;
}
#endif // SERVER_CPP
