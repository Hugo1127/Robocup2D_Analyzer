#ifndef PHYSICS_H
#define PHYSICS_H
#include <cmath>
#include <string>
#include <vector>
// Vector2D类
class Vector2D {
public:
    double x;
    double y;
    double vx;
    double vy;

    Vector2D(double x = 0.0, double y = 0.0, double vx = 0.0, double vy = 0.0);

    Vector2D pos() const;
    Vector2D abs() const;
    double absX() const;
    double absY() const;
    double dist(const Vector2D &point) const;
};


// Circle2D类
class Circle2D {
private:
    Vector2D center_pos;
    double radius;

public:
    Circle2D(const Vector2D &center_pos, double radius);

    Vector2D center() const;
    bool contains(const Vector2D &pos) const;
};


// Line2D类
class Line2D {
private:
    double a;
    double b;
    double c;

public:
    Line2D(const Vector2D &p1, const Vector2D &p2);

    double dist(const Vector2D &p) const;

    Vector2D intersection(const Line2D &line) const;

    bool isParallel(const Line2D &line) const;

    double getA() const;
    double getB() const;
    double getC() const;
};


// Rect2D类
class Rect2D {
private:
    Vector2D top_left;
    Vector2D bottom_right;

public:
    Rect2D(const Vector2D &top_left, const Vector2D &bottom_right);

    double top() const;
    double bottom() const;
    double left() const;
    double right() const;
    Vector2D center() const;

    bool contains(const Vector2D &point) const;
};


// BallObject类，继承自Vector2D
class BallObject : public Vector2D {
private:
    double ball_size;

public:
    BallObject(double x = 0.0, double y = 0.0, double vx = 0.0, double vy = 0.0, double ball_size = 0.5);

    double size() const;
};


// GameTime类
class GameTime {
public:
    int game_time;
    int t_over;
    GameTime(int game_time = 0, int t_over = 6000);
    void resetTime();
    int kick_off() const;
    int time_over() const;
    void addTime();
    int cycle() const;
    void setCycleTo(int c);
};
class Type {
public:
    Type(int score_l, int score_r);

    std::string type() const;

    int scoreLeft() const;
    int scoreRight() const;

    void _UpdatePlayMode(int cycle, const std::vector<std::pair<int, std::string>> &line);
    void UpdatePlayMode();

private:
    std::string mode;

    int score_l;
    int score_r;

    std::string __CheckPlayMode() const;
    void __ChangePlayModeToFoul();
    void __ChangePlayModeToOffSide();
    void __ChangePlayModeToPlayOn();
};
// 定义Action结构，含所有动作字段
struct Action {
    std::string kick;
    std::string dash;
    std::string turn;
    std::string attentionto;
    std::string change_view;
    std::string pointto;
    std::string turn_neck;
    std::string say;
    std::string tackle;

    Action()
        : kick(""), dash(""), turn(""), attentionto(""), change_view(""), pointto(""),
        turn_neck(""), say(""), tackle("") {}
};

class PlayerObject : public Vector2D {
private:
    int _unum;
    Vector2D _ball_pos;
    Action _action;

public:
    PlayerObject(double x=0.0, double y=0.0, double vx=0.0, double vy=0.0,
    int unum=0, const Vector2D &ball_pos=Vector2D(0.0, 0.0), const Action &action=Action());
    PlayerObject(int unum, const Action &action);

    bool isKickable() const;

    int unum() const;
    double distFromBall() const;

    const Action& action() const;

    std::string kick() const;
    std::string dash() const;
    std::string turn() const;
    std::string attentionto() const;
    std::string change_view() const;
    std::string pointto() const;
    std::string turn_neck() const;
    std::string say() const;
    std::string tackle() const;
};


#endif // PHYSICS_H
