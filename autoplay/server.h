#ifndef SERVER_H
#define SERVER_H
#include<QProcess>
#include<QThread>
class ProcessManager : public QThread
{
    Q_OBJECT
public:
    explicit ProcessManager(const QString serverpath, const QList<QPair<QString, QString>>& teamPairs, QObject *parent = nullptr);
    ~ProcessManager();
    bool checkRunning() {
    return isRunning;
    }
protected:
    void run() override;

private:
    QList<QPair<QString, QString>> m_teamPairs;
    bool isRunning=false ;
    const QString _serverpath;
};
#endif // SERVER_H
