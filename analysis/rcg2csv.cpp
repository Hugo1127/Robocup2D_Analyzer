#include "rcg2csv.h"
#include <fstream>
#include <sstream>
#include<QDebug>
#include <filesystem>
#include<QMessageBox>
#include <iostream>
Rcg2csv::Rcg2csv(World* wm) : wm(wm)
{
    our_name = wm->teamName();
    opp_name = wm->opponentTeamName();
}
std::string Rcg2csv::floatToStr(float value) const {
    std::ostringstream oss;
    oss.precision(6);
    oss << std::fixed << value;
    return oss.str();
}
void Rcg2csv::getRowListAndWrite(int maxcycle, const std::string& filename) {
    namespace fs = std::filesystem;

    // 创建输出文件路径
    fs::path dirPath(filename);
    fs::path outPath = dirPath / (wm->file_name + ".csv");
    std::string outputFilePath = outPath.string();

    // 打开文件并写入表头
    std::ofstream ofs(outputFilePath);
    if (!ofs.is_open()) {
        std::cerr << "Failed to open file: " << outputFilePath << "\n";
        return;
    }

    // 写入CSV表头
    ofs << "cycle,team_name,player_num,ball_x,ball_y,ball_vx,ball_vy,"
        << "player_x,player_y,player_vx,player_vy,kick,dash,turn,turn_neck,"
        << "tackle,change_view,attentionto,pointto,say\n";

    // 处理每个周期
    while (wm->time().kick_off() <= wm->time().cycle() && wm->time().cycle() <= maxcycle) {
        int curCycle = wm->time().cycle();

        // 处理我方球员
        for (int unum = 1; unum <= 11; ++unum) {
            PlayerObject player = wm->ourPlayer(unum, curCycle);
            // 写入我方球员数据
            ofs << curCycle << ','
                << our_name << ','
                << unum << ',';
            // 球的数据
            auto ballObj = wm->ball(curCycle);
            if (ballObj) {
                ofs << floatToStr(ballObj->x) << ','
                    << floatToStr(ballObj->y) << ','
                    << floatToStr(ballObj->vx) << ','
                    << floatToStr(ballObj->vy) << ',';
            } else {
                ofs << ",,,,";
            }
            // 球员位置和速度
            ofs << floatToStr(player.x) << ','
                << floatToStr(player.y) << ','
                << floatToStr(player.vx) << ','
                << floatToStr(player.vy) << ',';
            // 球员动作
            ofs << player.kick() << ','
                << player.dash() << ','
                << player.turn() << ','
                << player.turn_neck() << ','
                << player.tackle() << ','
                << player.change_view() << ','
                << player.attentionto() << ','
                << player.pointto() << ','
                << player.say() << '\n';
        }

        // 处理对方球员
        for (int unum = 1; unum <= 11; ++unum) {
            PlayerObject player = wm->theirPlayer(unum, curCycle);
            // 写入对方球员数据
            ofs << curCycle << ','
                << opp_name << ','
                << unum << ',';
            // 球的数据
            auto ballObj = wm->ball(curCycle);
            if (ballObj) {
                ofs << floatToStr(ballObj->x) << ','
                    << floatToStr(ballObj->y) << ','
                    << floatToStr(ballObj->vx) << ','
                    << floatToStr(ballObj->vy) << ',';
            } else {
                ofs << ",,,,";
            }
            // 球员位置和速度
            ofs << floatToStr(player.x) << ','
                << floatToStr(player.y) << ','
                << floatToStr(player.vx) << ','
                << floatToStr(player.vy) << ',';
            // 球员动作
            ofs << player.kick() << ','
                << player.dash() << ','
                << player.turn() << ','
                << player.turn_neck() << ','
                << player.tackle() << ','
                << player.change_view() << ','
                << player.attentionto() << ','
                << player.pointto() << ','
                << player.say() << '\n';
        }

        // 更新游戏状态
        wm->gameMode().UpdatePlayMode();
        wm->time().addTime();
        // 确保数据立即写入磁盘（可选，根据性能需求决定）
        ofs.flush();
    }
    ofs.close();
}
Logprocess::Logprocess(QObject *parent,const std::string& path,int cycle,const std::string& output_path)
    : QObject(parent),
    path(path),
    cycle(cycle),
    output_path(output_path){}
void Logprocess::dowork(){
    World wm = World(path);//rcg文件路径，同一个文件夹下rcl也要有
    Rcg2csv r1 = Rcg2csv(&wm);
    r1.getRowListAndWrite(cycle,output_path);//写个周期，比如10
    emit work_finished();
}
