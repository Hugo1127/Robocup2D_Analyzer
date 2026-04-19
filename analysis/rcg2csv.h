#ifndef RCG2CSV_H
#define RCG2CSV_H
#include <string>
#include <QThread>
#include <QObject>
#include"world_model.h"
class Rcg2csv {
public:
    explicit Rcg2csv(World* wm);
    void getRowListAndWrite(int maxcycle, const std::string& filename);

private:
    World* wm;
    std::string our_name;
    std::string opp_name;
    // 辅助函数，根据数据类型格式化转换为字符串
    std::string floatToStr(float value) const;
};
class Logprocess: public QObject{
Q_OBJECT
public:
explicit Logprocess(QObject *parent,const std::string& path,int cycle,const std::string& output_path);
signals:
    void work_finished();
public slots:
    void dowork();
private:
    std::string path;
    int cycle;
    std::string output_path;
    bool isworking=false;
};
#endif // RCG2CSV_H
