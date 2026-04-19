#ifndef WORLD_MODEL_H
#define WORLD_MODEL_H
#include"physics.h"
#include <string>
#include <vector>
class World {
public:
    explicit World(const std::string & file_path);

    std::string file_name;

    // -- 动作获取 --
    const Action* ourAction(int unum, int cycle = 0) const;
    const Action* theirAction(int unum, int cycle = 0) const;

    // -- 球队名获取 --
    const std::string& opponentTeamName() const;
    const std::string& teamName() const;
    void setTeamName(const std::string& teamName);

    // -- 球对象获取 --
    BallObject* ball(int cycle = 0) const;

    // -- 游戏时间对象引用 --
    const GameTime& time() const;
    GameTime& time();

    int fullstateTime() const;

    //
    Type& gameMode();
    float ourDefenseLineX() const;
    float ourOffenseLineX() const;
    float theirDefenseLineX() const;
    float theirOffenseLineX() const;
    bool existKickableOpponent();
    bool existKickableTeammate();

    // 最近踢球队伍
    std::string lastKickerSide();


    // 球员分割与解析辅助
    std::vector<PlayerObject> parsePlayers(const std::string& rcg_line, const std::string& pattern, int count) const;
    // 获取距离最近的对手、队友距离接口
    float getDistOpponentNearestTo(bool with_goalie, const Vector2D& point);
    float getDistOpponentNearestToBall(bool with_goalie = false);
    float getDistOpponentNearestToSelf(bool with_goalie = false);

    float getDistTeammateNearestTo(bool with_goalie, const Vector2D& point);
    float getDistTeammateNearestToBall(bool with_goalie = false);
    float getDistTeammateNearestToSelf(bool with_goalie = false);

    // 获取最近对手、队友对象接口
    PlayerObject getOpponentNearestTo(bool with_goalie, const Vector2D& point);
    PlayerObject getOpponentNearestToBall(bool with_goalie = false);
    PlayerObject getOpponentNearestToSelf(bool with_goalie = false);

    PlayerObject getTeammateNearestTo(bool with_goalie, const Vector2D& point);
    PlayerObject getTeammateNearestToBall(bool with_goalie = false);
    PlayerObject getTeammateNearestToSelf(bool with_goalie = false);

    // 通过编号获取指定周期的球员
    PlayerObject ourPlayer(int unum, int cycle = 0) const;
    PlayerObject theirPlayer(int unum, int cycle = 0) const;

private:
    std::vector<std::string> rcg;
    std::vector<std::vector<PlayerObject*>> rcl_l;
    std::vector<std::vector<PlayerObject*>> rcl_r;
    std::vector<std::pair<int, std::string>> playmode; // cycle和mode
    int left_team_score;
    int right_team_score;
    std::string last_kicker_side;
    std::string left_team_name;
    std::string right_team_name;
    Type game_mode;
    GameTime game_time;
};

#endif // WORLD_H
