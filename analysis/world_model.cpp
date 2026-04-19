#include "world_model.h"
#include <regex>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include<QDebug>
#include <cstdlib>
// 分割函数，分割字符串 s 以 delim 为分隔符，返回分割结果
std::vector<std::string> split(const std::string& s, const std::string& delim) {
    std::vector<std::string> elems;
    size_t pos = 0, lastPos = 0;
    while ((pos = s.find(delim, lastPos)) != std::string::npos) {
        elems.push_back(s.substr(lastPos, pos - lastPos));
        lastPos = pos + delim.size();
    }
    elems.push_back(s.substr(lastPos));
    return elems;
}
//优化版分割函数
std::vector<std::string_view> split_optimized(std::string_view s, std::string_view delim) {
    std::vector<std::string_view> elems;
    // 预分配内存（假设平均分割5段，避免频繁扩容）
    elems.reserve(12);

    const char* token_start = s.data();
    const char* const end_pos = s.data() + s.size();
    const size_t delim_len = delim.size();

    while (token_start < end_pos) {
        const char* delim_pos = std::search(token_start, end_pos, delim.begin(), delim.end());
        elems.emplace_back(token_start, delim_pos - token_start);

        if (delim_pos == end_pos) break;
        token_start = delim_pos + delim_len;
    }

    return elems;
}

World::World(const std::string& file_path)
    : left_team_score(0)
    , right_team_score(0)
    , last_kicker_side("left")
    , game_mode(0, 0)    // 默认初始化，后面会重设
    , game_time(0, 6000) // 默认初始化
{
    rcg.emplace_back("(show 0 ((b) 0.000 0.000");

    int max_cycle = fullstateTime() + 1;
    rcl_l.resize(max_cycle, std::vector<PlayerObject*>(12, nullptr));
    rcl_r.resize(max_cycle, std::vector<PlayerObject*>(12, nullptr));

    // 'playmode' 的类型是 vector<pair<int,string>>
    playmode.clear();

    // 文件名处理
    // 从路径中提取文件名
    std::string file_name = file_path.substr(file_path.find_last_of("/\\") + 1);
    file_name = file_name.substr(0, file_name.find_last_of('.')); // without extension
    this->file_name = file_name;

    // 使用正则表达式替换 ^[0-9]+- 为空字符串"
    //假设输入字符串是 "20250623190856-HfutEngine2025_3-vs-Cyrus2D_base11_1"，目标是删除左侧的数字前缀（20250623190856-）：
    std::regex rgx_prefix(R"(^[0-9]+-)");
    std::string left_part = file_name.substr(0, file_name.find("-vs-"));
    std::string right_part = file_name.substr(file_name.find("-vs-") + 4);

    std::smatch match;

    // 提取左侧队伍名字
    std::string left_vs = left_part;
    if (std::regex_search(left_vs, match, rgx_prefix)) {
        left_vs = std::regex_replace(left_vs, rgx_prefix, "");
    }
    // 提取队伍名字
    std::regex rgx_teamname_left(R"(([^_]+))");
    std::string candidate_left_team = "";
    {
        std::smatch m2;
        std::regex re_split(R"(_[0-9]+)");
        if (std::regex_search(left_vs, m2, re_split)) {
            candidate_left_team = left_vs.substr(0, m2.position(0));
        }
        else {
            candidate_left_team = left_vs;
        }
    }
    this->left_team_name = candidate_left_team;

    //提取右侧队伍名字
    std::smatch m_right;
    std::regex re_split_right(R"(_[0-9]+)");
    std::string right_team_candidate;
    if (std::regex_search(right_part, m_right, re_split_right)) {
        right_team_candidate = right_part.substr(0, m_right.position(0));
    }
    else {
        right_team_candidate = right_part;
    }
    this->right_team_name = right_team_candidate;

    // 提取进球数
    // 左侧队伍进球数为队名后面的数字，HfutEngine2025_3-vs-Cyrus2D_base11_1中为3:1
    {
        std::string left_score_str;
        auto pos = left_part.find(this->left_team_name + "_");
        if (pos != std::string::npos) {
            left_score_str = left_part.substr(pos + this->left_team_name.size() + 1);
            this->left_team_score = std::stoi(left_score_str);
        }
        else {
            this->left_team_score = 0;
        }
    }

    // 右侧球队进球数
    {
        std::string right_score_str;
        auto pos = right_part.find(this->right_team_name + "_");
        if (pos != std::string::npos) {
            right_score_str = right_part.substr(pos + this->right_team_name.size() + 1);
            this->right_team_score = std::stoi(right_score_str);
        }
        else {
            this->right_team_score = 0;
        }
    }
    // initialize game mode
    game_mode = Type(this->left_team_score, this->right_team_score);
    // initialize game time
    game_time = GameTime(0, 6000);
    last_kicker_side = "left";
    std::string rcgfile = file_path.substr(0, file_path.size() - 3) + "rcg";
    std::string rclfile = file_path.substr(0, file_path.size() - 3) + "rcl";
    std::ifstream rcg_stream(rcgfile);
    if (!rcg_stream) {
        std::cerr << "Error opening file: " << rcgfile << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(rcg_stream, line)) {
        if (line.find("show") != std::string::npos) {
            int last_cycle = 0;
            if (!rcg.empty()) {
                std::istringstream iss(rcg.back());
                std::string tmp;
                iss >> tmp; // (show
                iss >> last_cycle;
                //qDebug()<<"99";//done
            }

            int current_cycle = 0;
            {
                std::istringstream iss(line);
                std::string tmp;
                iss >> tmp; // (show
                iss >> current_cycle;
                //qDebug()<<"99";//done
            }

            if (last_cycle < current_cycle) {
                rcg.push_back(line);
                //qDebug()<<"99";
                game_time.addTime();
                if (current_cycle == 2999) {
                    rcg.emplace_back("(show 3000 ((b) 0.000 0.000");
                    game_time.addTime();
                    //qDebug()<<"99";
                }
            }
        }
        else if (line.find("playmode") != std::string::npos) {
            std::istringstream iss(line);
            std::string tmp;
            iss >> tmp; // (playmode
            int cycle;
            std::string mode;
            //qDebug()<<"99";
            if (!(iss >> cycle >> mode)) {
                // 解析失败，跳过或报错
                continue;
            }
            if (!mode.empty() && mode.back() == ')') {
                mode.pop_back();
            }
            playmode.emplace_back(cycle, mode);
        }
        //qDebug()<<"99";
    }
    rcg_stream.close();
    game_time.t_over = this->time().cycle();
    BallObject *b1=ball();
    if (b1 == nullptr) {
        game_time.t_over -= 1;
    }
    delete b1;
    game_time.resetTime();
    // 打开rcl文件
    std::ifstream rcl_stream(rclfile);
    if (!rcl_stream) {
        std::cerr << "Error opening file: " << rclfile << std::endl;
        std::exit(EXIT_FAILURE);
    }

    while (std::getline(rcl_stream, line)) {
        // line example:
        // cycle,teamname_unum: action...
        // parse cycle as int before comma
        size_t pos_comma = line.find(',');
        if (pos_comma == std::string::npos) continue;
        int rcl_cycle = std::stoi(line.substr(0, pos_comma));
        if (rcl_cycle < 1) continue;
        Action action;
        // check which team line belongs to
        if (line.find(left_team_name) != std::string::npos && line.find("Coach") == std::string::npos) {
            // get unum
            // line contains: left_team_name + "_<unum>: "
            size_t pos_team = line.find(left_team_name);
            size_t pos_colon = line.find(": ", pos_team);
            size_t pos_underscore = line.find('_', pos_team);
            if (pos_colon == std::string::npos || pos_underscore == std::string::npos || pos_underscore > pos_colon) {
                continue; // invalid format
            }
            //qDebug()<<"4";//done
            //int rcl_unum = std::stoi(line.substr(pos_underscore + 1, pos_colon - pos_underscore -1));
            std::vector<std::string> parts = split(line, left_team_name);
            if (parts.size() < 2) {
                std::cerr << "Error: left_team_name not found or no second part.\n";
            }

            std::vector<std::string> parts2 = split(parts[1], ": ");
            if (parts2.empty()) {
                std::cerr << "Error: no part after splitting by ': '\n";
            }

            std::vector<std::string> parts3 = split(parts2[0], "_");
            if (parts3.size() < 2) {
                std::cerr << "Error: underscore split has less than 2 parts";
            }

            int rcl_unum = std::stoi(parts3[1]);
            //qDebug()<<rcl_unum;//not here
            if (rcl_cycle == 6000) {
                rcl_l[rcl_cycle][rcl_unum] = new PlayerObject(rcl_unum, action);
                continue;
            }

            // action string after ": "
            std::string rcl_action = line.substr(pos_colon + 2);

            // parse actions one by one, similar to python code

            if (rcl_action.find("kick") != std::string::npos) {
                size_t pos_kick = rcl_action.find("kick");
                std::istringstream iss(rcl_action.substr(pos_kick + 4));
                std::string kick_val;
                std::getline(iss, kick_val, ')');
                action.kick = kick_val;
            }

            if (rcl_action.find("dash") != std::string::npos) {
                size_t pos_dash = rcl_action.find("dash");
                std::istringstream iss(rcl_action.substr(pos_dash + 4));
                std::string dash_val;
                std::getline(iss, dash_val, ')');
                action.dash = dash_val;
            }
            if (rcl_action.find("turn_neck") != std::string::npos) {
                size_t pos_turn_neck = rcl_action.find("turn_neck");
                std::istringstream iss(rcl_action.substr(pos_turn_neck + 9));
                float turn_neck_val;
                iss >> turn_neck_val;
                action.turn_neck = std::to_string(turn_neck_val);
            }
            if (rcl_action.find("pointto") != std::string::npos) {
                size_t pos_pointto = rcl_action.find("pointto");
                std::istringstream iss(rcl_action.substr(pos_pointto + 7));
                std::string pointto_val;
                iss >> pointto_val;
                if (!pointto_val.empty() && pointto_val.back() == ')') {
                    pointto_val.pop_back();
                }
                action.pointto = pointto_val;
            }
            if (rcl_action.find("say") != std::string::npos) {
                size_t pos_say = rcl_action.find("say");
                // find quoted string after say
                size_t quote1 = rcl_action.find('"', pos_say);
                if (quote1 != std::string::npos) {
                    size_t quote2 = rcl_action.find('"', quote1 + 1);
                    if (quote2 != std::string::npos) {
                        action.say = rcl_action.substr(quote1 + 1, quote2 - quote1 - 1);
                    }
                }
            }
            if (rcl_action.find("turn") != std::string::npos) {
                size_t pos_turn = rcl_action.find("turn");
                std::istringstream iss(rcl_action.substr(pos_turn + 4));
                float turn_val;
                iss >> turn_val;
                //action.turn = new float(turn_val);
                action.turn = std::to_string(turn_val);
            }
            if (rcl_action.find("tackle") != std::string::npos) {
                size_t pos_tackle = rcl_action.find("tackle");
                std::string tackle_str = rcl_action.substr(pos_tackle + 6);
                // remove trailing ')'
                size_t pos_paren = tackle_str.find(')');
                if (pos_paren != std::string::npos) {
                    tackle_str = tackle_str.substr(0, pos_paren);
                }
                // remove leading spaces
                tackle_str = std::regex_replace(tackle_str, std::regex("^ +"), "");
                action.tackle = tackle_str;
            }
            if (rcl_action.find("change_view") != std::string::npos) {
                size_t pos = rcl_action.find("change_view");
                size_t start = rcl_action.find(' ', pos);
                size_t end = rcl_action.find(')', pos);
                if (start != std::string::npos && end != std::string::npos && end > start) {
                    action.change_view = rcl_action.substr(start + 1, end - start - 1);
                }
            }
            if (rcl_action.find("attentionto") != std::string::npos) {
                size_t pos = rcl_action.find("attentionto");
                size_t start = rcl_action.find(' ', pos);
                size_t end = rcl_action.find(')', pos);
                if (start != std::string::npos && end != std::string::npos && end > start) {
                    action.attentionto = rcl_action.substr(start + 1, end - start - 1);
                    // Trim leading and trailing whitespace
                    action.attentionto.erase(0, action.attentionto.find_first_not_of(" \t"));
                    action.attentionto.erase(action.attentionto.find_last_not_of(" \t") + 1);
                }
            }
            rcl_l[rcl_cycle][rcl_unum] = new PlayerObject(rcl_unum, action);

        }
        else if (line.find(right_team_name) != std::string::npos && line.find("Coach") == std::string::npos) {
            size_t pos_team = line.find(right_team_name);
            size_t pos_colon = line.find(": ", pos_team);
            size_t pos_underscore = line.find('_', pos_team);
            if (pos_colon == std::string::npos || pos_underscore == std::string::npos || pos_underscore > pos_colon) {
                continue; // invalid format
            }
            std::vector<std::string> parts = split(line, right_team_name);
            if (parts.size() < 2) {
                std::cerr << "Error: right_team_name not found or no second part.\n";
            }

            std::vector<std::string> parts2 = split(parts[1], ": ");
            if (parts2.empty()) {
                std::cerr << "Error: no part after splitting by ': '\n";
            }

            std::vector<std::string> parts3 = split(parts2[0], "_");
            if (parts3.size() < 2) {
                std::cerr << "Error: underscore split has less than 2 parts";
            }
            int rcl_unum = std::stoi(parts3[1]);
            if (rcl_cycle == 6000) {
                rcl_r[rcl_cycle][rcl_unum] = new PlayerObject(rcl_unum, action);
                continue;
            }

            std::string rcl_action = line.substr(pos_colon + 2);

            if (rcl_action.find("kick") != std::string::npos) {
                size_t pos_kick = rcl_action.find("kick");
                std::istringstream iss(rcl_action.substr(pos_kick + 4));
                std::string kick_val;
                std::getline(iss, kick_val, ')');
                action.kick = kick_val;
            }

            if (rcl_action.find("dash") != std::string::npos) {
                size_t pos_dash = rcl_action.find("dash");
                std::istringstream iss(rcl_action.substr(pos_dash + 4));
                std::string dash_val;
                std::getline(iss, dash_val, ')');
                action.dash = dash_val;
            }
            if (rcl_action.find("turn_neck") != std::string::npos) {
                size_t pos_turn_neck = rcl_action.find("turn_neck");
                std::istringstream iss(rcl_action.substr(pos_turn_neck + 9));
                float turn_neck_val;
                iss >> turn_neck_val;
                action.turn_neck = std::to_string(turn_neck_val);

            }
            if (rcl_action.find("pointto") != std::string::npos) {
                size_t pos_pointto = rcl_action.find("pointto");
                std::istringstream iss(rcl_action.substr(pos_pointto + 7));
                std::string pointto_val;
                iss >> pointto_val;
                if (!pointto_val.empty() && pointto_val.back() == ')') {
                    pointto_val.pop_back();
                }
                action.pointto = pointto_val;
            }
            if (rcl_action.find("say") != std::string::npos) {
                size_t pos_say = rcl_action.find("say");
                // find quoted string after say
                size_t quote1 = rcl_action.find('"', pos_say);
                if (quote1 != std::string::npos) {
                    size_t quote2 = rcl_action.find('"', quote1 + 1);
                    if (quote2 != std::string::npos) {
                        action.say = rcl_action.substr(quote1 + 1, quote2 - quote1 - 1);
                    }
                }
            }
            if (rcl_action.find("turn") != std::string::npos) {
                size_t pos_turn = rcl_action.find("turn");
                std::istringstream iss(rcl_action.substr(pos_turn + 4));
                float turn_val;
                iss >> turn_val;
                //action.turn = new float(turn_val);
                action.turn = std::to_string(turn_val);
            }
            if (rcl_action.find("tackle") != std::string::npos) {
                size_t pos_tackle = rcl_action.find("tackle");
                std::string tackle_str = rcl_action.substr(pos_tackle + 6);
                // remove trailing ')'
                size_t pos_paren = tackle_str.find(')');
                if (pos_paren != std::string::npos) {
                    tackle_str = tackle_str.substr(0, pos_paren);
                }
                // remove leading spaces
                tackle_str = std::regex_replace(tackle_str, std::regex("^ +"), "");
                action.tackle = tackle_str;
            }
            if (rcl_action.find("change_view") != std::string::npos) {
                size_t pos = rcl_action.find("change_view");
                size_t start = rcl_action.find(' ', pos);
                size_t end = rcl_action.find(')', pos);
                if (start != std::string::npos && end != std::string::npos && end > start) {
                    action.change_view = rcl_action.substr(start + 1, end - start - 1);
                }
            }
            if (rcl_action.find("attentionto") != std::string::npos) {
                size_t pos = rcl_action.find("attentionto");
                size_t start = rcl_action.find(' ', pos);
                size_t end = rcl_action.find(')', pos);
                if (start != std::string::npos && end != std::string::npos && end > start) {
                    action.attentionto = rcl_action.substr(start + 1, end - start - 1);
                    // Trim leading and trailing whitespace
                    action.attentionto.erase(0, action.attentionto.find_first_not_of(" \t"));
                    action.attentionto.erase(action.attentionto.find_last_not_of(" \t") + 1);
                }
            }
            rcl_r[rcl_cycle][rcl_unum] = new PlayerObject(rcl_unum, action);
        }
    }
    rcl_stream.close();

}

int World::fullstateTime() const {
    return 6000;
}
const Action* World::ourAction(int unum, int cycle) const {
    if (cycle == 0)
        cycle = game_time.cycle();

    if (cycle < 0 || cycle >= static_cast<int>(rcl_l.size()) ||
        unum < 0 || unum >= static_cast<int>(rcl_l[cycle].size()) ||
        rcl_l[cycle][unum] == nullptr)
        return nullptr;

    return &(rcl_l[cycle][unum]->action());
}

const Action* World::theirAction(int unum, int cycle) const {
    if (cycle == 0)
        cycle = game_time.cycle();

    if (cycle < 0 || cycle >= static_cast<int>(rcl_r.size()) ||
        unum < 0 || unum >= static_cast<int>(rcl_r[cycle].size()) ||
        rcl_r[cycle][unum] == nullptr)
        return nullptr;

    return &(rcl_r[cycle][unum]->action());
}

const std::string& World::opponentTeamName() const {
    return right_team_name;
}

const std::string& World::teamName() const {
    return left_team_name;
}

void World::setTeamName(const std::string& teamName) {
    left_team_name = teamName;
}

BallObject* World::ball(int cycle) const {
    if (cycle == 0)
        cycle = game_time.cycle();

    if (cycle < 0 || cycle >= static_cast<int>(rcg.size()))
        return nullptr;

    const std::string& line = rcg[cycle];
    size_t pos = line.find("((b) ");
    if (pos == std::string::npos)
        return nullptr;
    pos += 5; // 跳过 "((b) "
    std::istringstream iss(line.substr(pos));
    float ball_x, ball_y, ball_vx, ball_vy;
    if (!(iss >> ball_x >> ball_y >> ball_vx >> ball_vy))
        return nullptr;
    // 球半径0.5
    return new BallObject(ball_x, ball_y, ball_vx, ball_vy, 0.5f);
}

const GameTime& World::time() const {
    return game_time;
}
GameTime& World::time() {
    return game_time;
}

// ---- gameMode() ----
Type& World::gameMode()
{
    game_mode._UpdatePlayMode(game_time.cycle(), playmode);
    return game_mode;
}

float World::ourDefenseLineX() const
{
    if (rcg.size() <= static_cast<size_t>(game_time.cycle()))
        return 65535.0f;

    const std::string& match = rcg[game_time.cycle()];
    // 正则匹配l队球员数据，pattern同python对应正则
    std::regex pattern(R"(\(\(l \d{1,2}\))");
    // 将字符串按pattern拆分。C++直接使用std::regex_token_iterator分割空字符串前缀，所以这里简单用split实现
    // 这里不作复杂实现，简单拆分用自定义函数
    std::vector<std::string> splits;
    size_t start = 0;
    std::sregex_token_iterator iter(match.begin(), match.end(), pattern, -1);
    std::sregex_token_iterator end;
    for (; iter != end; ++iter)
        splits.push_back(iter->str());

    // 产生splits[1..11]为球员数据(segment)
    // 兼容访问，保证够多
    if (splits.size() < 12)
        return 65535.0f;

    // 创建球员容器，首个无用，下标1..11有效
    std::vector<PlayerObject> teammate;
    teammate.reserve(12);
    teammate.emplace_back(0.0f, 0.0f, 0); // dummy

    for (int i = 1; i <= 11; ++i) {
        std::istringstream iss(splits[i]);
        // splits[i] 格式例如："(l 2) x y ..."
        // 我们取第3和第4个元素，即x,y坐标
        std::string dummy;
        int unum_dummy;

        float x, y;
        iss >> dummy >> unum_dummy >> x >> y;
        // 如果格式不同，需根据实际修调整

        teammate.emplace_back(x, y, i);
    }

    float defense_line = 65535.0f;
    for (int i = 2; i <= 11; ++i) {
        if (teammate[i].pos().x < defense_line)
            defense_line = teammate[i].pos().x;
    }

    return defense_line;
}

// ---- ourOffenseLineX ----
// 计算我方进攻线X坐标（最大X）
float World::ourOffenseLineX() const
{
    if (rcg.size() <= static_cast<size_t>(game_time.cycle()))
        return -65535.0f;

    const std::string& match = rcg[game_time.cycle()];
    std::regex pattern(R"(\(\(l \d{1,2}\))");

    std::vector<std::string> splits;
    std::sregex_token_iterator iter(match.begin(), match.end(), pattern, -1);
    std::sregex_token_iterator end;
    for (; iter != end; ++iter)
        splits.push_back(iter->str());

    if (splits.size() < 12)
        return -65535.0f;

    std::vector<PlayerObject> teammate;
    teammate.reserve(12);
    teammate.emplace_back(0.0f, 0.0f, 0);

    for (int i = 1; i <= 11; ++i) {
        std::istringstream iss(splits[i]);
        std::string dummy;
        int unum_dummy;
        float x, y;
        iss >> dummy >> unum_dummy >> x >> y;

        teammate.emplace_back(x, y, i);
    }

    float offense_line = -65535.0f;
    for (int i = 2; i <= 11; ++i) {
        if (teammate[i].pos().x > offense_line)
            offense_line = teammate[i].pos().x;
    }

    return offense_line;
}

// ---- theirDefenseLineX ----
// 计算对方防线X坐标（最大X）
float World::theirDefenseLineX() const
{
    if (rcg.size() <= static_cast<size_t>(game_time.cycle()))
        return -65535.0f;

    const std::string& match = rcg[game_time.cycle()];
    std::regex pattern(R"(\(\(l \d{1,2}\))");

    std::vector<std::string> splits;
    std::sregex_token_iterator iter(match.begin(), match.end(), pattern, -1);
    std::sregex_token_iterator end;
    for (; iter != end; ++iter)
        splits.push_back(iter->str());

    if (splits.size() < 12)
        return -65535.0f;

    std::vector<PlayerObject> opponent;
    opponent.reserve(12);
    opponent.emplace_back(0.0f, 0.0f, 0);

    for (int i = 1; i <= 11; ++i) {
        std::istringstream iss(splits[i]);
        std::string dummy;
        int unum_dummy;
        float x, y;
        iss >> dummy >> unum_dummy >> x >> y;

        opponent.emplace_back(x, y, i);
    }

    float defense_line = -65535.0f;
    for (int i = 2; i <= 11; ++i) {
        if (opponent[i].pos().x > defense_line)
            defense_line = opponent[i].pos().x;
    }

    return defense_line;
}

// ---- theirOffenseLineX ----
// 计算对方进攻线X坐标（最小X）
float World::theirOffenseLineX() const
{
    if (rcg.size() <= static_cast<size_t>(game_time.cycle()))
        return 65535.0f;

    const std::string& match = rcg[game_time.cycle()];
    std::regex pattern(R"(\(\(l \d{1,2}\))");

    std::vector<std::string> splits;
    std::sregex_token_iterator iter(match.begin(), match.end(), pattern, -1);
    std::sregex_token_iterator end;
    for (; iter != end; ++iter)
        splits.push_back(iter->str());

    if (splits.size() < 12)
        return 65535.0f;

    std::vector<PlayerObject> opponent;
    opponent.reserve(12);
    opponent.emplace_back(0.0f, 0.0f, 0);

    for (int i = 1; i <= 11; ++i) {
        std::istringstream iss(splits[i]);
        std::string dummy;
        int unum_dummy;
        float x, y;
        iss >> dummy >> unum_dummy >> x >> y;

        opponent.emplace_back(x, y, i);
    }

    float offense_line = 65535.0f;
    for (int i = 2; i <= 11; ++i) {
        if (opponent[i].pos().x < offense_line)
            offense_line = opponent[i].pos().x;
    }

    return offense_line;
}

bool World::existKickableOpponent()
{
    float dist_OppToBall = getDistOpponentNearestToBall(true);
    float dist_MateToBall = getDistTeammateNearestToBall(true);
    return (dist_MateToBall > dist_OppToBall && dist_OppToBall < 1.5f);
}

bool World::existKickableTeammate()
{
    float dist_OppToBall = getDistOpponentNearestToBall(true);
    float dist_MateToBall = getDistTeammateNearestToBall(true);
    return (dist_MateToBall < dist_OppToBall && dist_MateToBall < 1.5f);
}

std::string World::lastKickerSide()
{
    if (existKickableOpponent())
        last_kicker_side = "right";
    else if (existKickableTeammate())
        last_kicker_side = "left";
    return last_kicker_side;
}

// 辅助：解析符合pattern的球员，返回PlayerObject数组（共count个球员）
std::vector<PlayerObject> World::parsePlayers(const std::string& rcg_line, const std::string& pattern, int count) const
{
    std::regex re_pattern(pattern);
    std::vector<PlayerObject> players;
    players.emplace_back(0.0f, 0.0f, 0); // index 0占位

    std::sregex_token_iterator iter(rcg_line.begin(), rcg_line.end(), re_pattern, -1);
    std::sregex_token_iterator end;

    std::vector<std::string> splits;
    for (; iter != end; ++iter)
        splits.push_back(iter->str());

    if ((int)splits.size() < count + 1) {
        // 需要的数量太少，直接返回空队伍，或debug提示
        return players;
    }

    for (int i = 1; i <= count; ++i) {
        std::istringstream iss(splits[i]);
        std::string dummy;
        float x = 0.0f, y = 0.0f, some_val1 = 0.0f, some_val2 = 0.0f;
        // 解析格式略微假设，确保顺序正确
        // 如格式: (l 1) x y val1 val2 ...
        int unum = i;

        iss >> dummy >> unum >> x >> y >> some_val1 >> some_val2;

        // 构造PlayerObject，如有ball_pos和action需传入
        players.emplace_back(x, y, unum);
    }
    return players;
}

float World::getDistOpponentNearestTo(bool with_goalie, const Vector2D& point)
{
    if ((int)rcg.size() <= game_time.cycle()) return std::numeric_limits<float>::max();
    std::string pattern = (R"(\(\(l \d{1,2}\))");
    std::string match = rcg[game_time.cycle()];
    auto opponent = parsePlayers(match, pattern, 11);
    int minimum = with_goalie ? 1 : 2;
    float d = std::numeric_limits<float>::max();

    for (int i = minimum; i <= 11 && i < (int)opponent.size(); ++i) {
        float dist = point.dist(opponent[i].pos());
        if (dist < d) d = dist;
    }
    return d;
}

float World::getDistOpponentNearestToBall(bool with_goalie)
{
    BallObject* ball_obj = ball();
    if (!ball_obj) return std::numeric_limits<float>::max();

    return getDistOpponentNearestTo(with_goalie, ball_obj->pos());
}

float World::getDistOpponentNearestToSelf(bool with_goalie)
{
    // TODO: 需定义自己位置point，示例如球员本身、相机等
    return std::numeric_limits<float>::max();
}

float World::getDistTeammateNearestTo(bool with_goalie, const Vector2D& point)
{
    if ((int)rcg.size() <= game_time.cycle()) return std::numeric_limits<float>::max();
    std::string pattern = (R"(\(\(l \d{1,2}\))");
    std::string match = rcg[game_time.cycle()];
    auto teammate = parsePlayers(match, pattern, 11);
    int minimum = with_goalie ? 1 : 2;
    float d = std::numeric_limits<float>::max();

    for (int i = minimum; i <= 11 && i < (int)teammate.size(); ++i) {
        float dist = point.dist(teammate[i].pos());
        if (dist < d) d = dist;
    }
    return d;
}

float World::getDistTeammateNearestToBall(bool with_goalie)
{
    BallObject* ball_obj = ball();
    if (!ball_obj) return std::numeric_limits<float>::max();
    return getDistTeammateNearestTo(with_goalie, ball_obj->pos());
}

float World::getDistTeammateNearestToSelf(bool with_goalie)
{
    // TODO: 需定义自己位置point，示例如球员本身、相机等
    return std::numeric_limits<float>::max();
}

PlayerObject World::getOpponentNearestTo(bool with_goalie, const Vector2D& point)
{
    if ((int)rcg.size() <= game_time.cycle()) return PlayerObject();
    std::string pattern = (R"(\(\(l \d{1,2}\))");
    std::string match = rcg[game_time.cycle()];
    auto opponent = parsePlayers(match, pattern, 11);
    int minimum = with_goalie ? 1 : 2;

    float d = std::numeric_limits<float>::max();
    PlayerObject nearest_opponent;

    for (int i = minimum; i <= 11 && i < (int)opponent.size(); ++i) {
        float dist = point.dist(opponent[i].pos());
        if (dist < d) {
            d = dist;
            nearest_opponent = opponent[i];
        }
    }
    return nearest_opponent;
}

PlayerObject World::getOpponentNearestToBall(bool with_goalie)
{
    BallObject* ball_obj = ball();
    if (!ball_obj) return PlayerObject();
    return getOpponentNearestTo(with_goalie, ball_obj->pos());
}

PlayerObject World::getOpponentNearestToSelf(bool with_goalie)
{
    // TODO: 需定义自己位置point，示例如球员自己、相机位置等
    return PlayerObject();
}

PlayerObject World::getTeammateNearestTo(bool with_goalie, const Vector2D& point)
{
    if ((int)rcg.size() <= game_time.cycle()) return PlayerObject();
    //R"(\(\(l \d\))"原有的表达式
    std::string pattern = (R"(\(\(l \d{1,2}\))");
    std::string match = rcg[game_time.cycle()];
    auto teammate = parsePlayers(match, pattern, 11);

    int minimum = with_goalie ? 1 : 2;

    float d = std::numeric_limits<float>::max();
    PlayerObject nearest_teammate;

    for (int i = minimum; i <= 11 && i < (int)teammate.size(); ++i) {
        float dist = point.dist(teammate[i].pos());
        if (dist < d) {
            d = dist;
            nearest_teammate = teammate[i];
        }
    }
    return nearest_teammate;
}

PlayerObject World::getTeammateNearestToBall(bool with_goalie)
{
    BallObject* ball_obj = ball();
    if (!ball_obj) return PlayerObject();

    return getTeammateNearestTo(with_goalie, ball_obj->pos());
}

PlayerObject World::getTeammateNearestToSelf(bool with_goalie)
{
    // TODO: 需定义自己位置point，示例如球员自己、相机位置等
    return PlayerObject();
}

PlayerObject World::ourPlayer(int unum, int cycle) const
{
    if (cycle == 0)
        cycle = game_time.cycle();
    if (unum < 1 || unum > 11)
        return PlayerObject();
    if ((int)rcg.size() <= cycle) {
        //qDebug() << "8";
        return PlayerObject();
    }
    std::string match = rcg[cycle];
    //qDebug()<<rcg[cycle];

    std::regex re(R"(\(\(l \d{1,2}\))");
    std::sregex_token_iterator iter(match.begin(), match.end(), re, -1);
    std::sregex_token_iterator end;
    std::vector<std::string> splits;
    for (; iter != end; ++iter)
        splits.push_back(*iter);
    qDebug() << "splits.size()=" << splits.size();
    if (unum >= (int)splits.size()) {
        qDebug() << "7";//error
        return PlayerObject();
    }
    std::istringstream iss(splits[unum]);
    std::vector<std::string> vals;
    std::string tmp;
    while (iss >> tmp)
        vals.push_back(tmp);
    // Python中用索引2~5取得浮点数，C++同理
    float x = std::stof(vals[2]);
    float y = std::stof(vals[3]);
    float val1 = std::stof(vals[4]);
    float val2 = std::stof(vals[5]);
    return PlayerObject(x, y, val1, val2, unum, this->ball(cycle)->pos(), *(this->ourAction(unum, cycle)));
}
// 通过unum取得theirPlayer
PlayerObject World::theirPlayer(int unum, int cycle) const
{
    if (cycle == 0)
        cycle = game_time.cycle();
    if (unum < 1 || unum > 11)
        return PlayerObject();
    if ((int)rcg.size() <= cycle) {
        return PlayerObject();
    }
    std::string match = rcg[cycle];
    std::regex re(R"(\(\(l \d{1,2}\))");
    //R"(\(\(l \d\))"
    //R"(\(\(l \d{1,2}\))"
    std::sregex_token_iterator iter(match.begin(), match.end(), re, -1);
    std::sregex_token_iterator end;
    std::vector<std::string> splits;
    for (; iter != end; ++iter)
        splits.push_back(*iter);
    if (unum >= (int)splits.size()) {
        return PlayerObject();
    }
    std::istringstream iss(splits[unum]);
    std::vector<std::string> vals;
    std::string tmp;
    while (iss >> tmp)
        vals.push_back(tmp);
    // Python中用索引2~5取得浮点数，C++同理
    float x = std::stof(vals[2]);
    float y = std::stof(vals[3]);
    float val1 = std::stof(vals[4]);
    float val2 = std::stof(vals[5]);
    return PlayerObject(x, y, val1, val2, unum, this->ball(cycle)->pos(), *(this->theirAction(unum, cycle)));
}

