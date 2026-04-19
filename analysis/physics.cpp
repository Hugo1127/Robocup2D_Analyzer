#include "physics.h"
// Vector2D实现
Vector2D::Vector2D(double x_, double y_, double vx_, double vy_)
    : x(x_), y(y_), vx(vx_), vy(vy_) {}

Vector2D Vector2D::pos() const {
    return Vector2D(x, y);
}

Vector2D Vector2D::abs() const {
    return Vector2D(std::fabs(x), std::fabs(y));
}

double Vector2D::absX() const {
    return std::fabs(x);
}

double Vector2D::absY() const {
    return std::fabs(y);
}

double Vector2D::dist(const Vector2D &point) const {
    double dx = x - point.x;
    double dy = y - point.y;
    return std::sqrt(dx*dx + dy*dy);
}


// Circle2D实现
Circle2D::Circle2D(const Vector2D &center_pos_, double radius_)
    : center_pos(center_pos_), radius(radius_) {}

Vector2D Circle2D::center() const {
    return center_pos;
}

bool Circle2D::contains(const Vector2D &pos) const {
    //若在圆内返回true，不在返回false
    return center_pos.dist(pos) <= radius;
}


// Line2D实现
Line2D::Line2D(const Vector2D &p1, const Vector2D &p2) {
    a = -(p2.y - p1.y);
    b = p2.x - p1.x;
    c = -a * p1.x - b * p1.y;
}

double Line2D::dist(const Vector2D &p) const {
    return std::fabs((a * p.x + b * p.y + c) / std::sqrt(a*a + b*b));
}

Vector2D Line2D::intersection(const Line2D &line) const {
    double denom = a * line.b - line.a * b;
    if (std::fabs(denom) < 1e-10) {
        // 平行无交点
        return Vector2D(-100.0, -100.0);
    }

    double intersection_x = (b * line.c - line.b * c) / denom;
    double intersection_y = (line.a * c - a * line.c) / denom;

    // 判断边界，球场大小为（105x68）
    if (intersection_x > 52.5 || intersection_x < -52.5 ||
        intersection_y > 34.0 || intersection_y < -34.0) {
        return Vector2D(-100.0, -100.0);
    }

    return Vector2D(intersection_x, intersection_y);
}

bool Line2D::isParallel(const Line2D &line) const {
    //由于是浮点数加减，有误差
    return std::fabs(a * line.b - line.a * b) <= 0.051;
}

double Line2D::getA() const { return a; }
double Line2D::getB() const { return b; }
double Line2D::getC() const { return c; }


// Rect2D实现
Rect2D::Rect2D(const Vector2D &top_left_, const Vector2D &bottom_right_)
    : top_left(top_left_), bottom_right(bottom_right_) {}

double Rect2D::top() const { return top_left.y; }
double Rect2D::bottom() const { return bottom_right.y; }
double Rect2D::left() const { return top_left.x; }
double Rect2D::right() const { return bottom_right.x; }

Vector2D Rect2D::center() const {
    return Vector2D((left() + right()) * 0.5, (top() + bottom()) * 0.5);
}

bool Rect2D::contains(const Vector2D &point) const {
    return (left() <= point.x && point.x <= right()) &&
           (top() <= point.y && point.y <= bottom());
}


// BallObject实现
BallObject::BallObject(double x, double y, double vx, double vy, double ball_size_)
    : Vector2D(x, y, vx, vy), ball_size(ball_size_) {}

double BallObject::size() const {
    return ball_size;
}


// GameTime实现
GameTime::GameTime(int game_time_, int t_over_)
    : game_time(game_time_), t_over(t_over_) {}

void GameTime::resetTime() {
    game_time = 1;
}

int GameTime::kick_off() const {
    return 1;
}

int GameTime::time_over() const {
    return t_over;
}

void GameTime::addTime()  {
    game_time += 1;
    if (game_time == 3000) {
        game_time = 3001;
    }
}

int GameTime::cycle() const {
    return game_time;
}

void GameTime::setCycleTo(int c) {
    // 时间范围放最大到6000
    if (c >= 1 && c <= 6000) {
        game_time = c;
    }
}
Type::Type(int score_l_, int score_r_) : mode("kick_off"), score_l(score_l_), score_r(score_r_) {}

std::string Type::type() const {
    return mode;
}

int Type::scoreLeft() const {
    return score_l;
}

int Type::scoreRight() const {
    return score_r;
}

void Type::_UpdatePlayMode(int cycle, const std::vector<std::pair<int, std::string>> &line) {
    for(const auto &i : line) {
        if(i.first <= cycle) {
            mode = i.second;
        } else if(i.first > cycle) {
            break;
        }
    }
}
std::string Type::__CheckPlayMode() const {
    return mode;
}

void Type::__ChangePlayModeToFoul() {
    mode = "foul_charge";
}

void Type::__ChangePlayModeToOffSide() {
    mode = "offside";
}

void Type::__ChangePlayModeToPlayOn() {
    mode = "play_on";
}

PlayerObject::PlayerObject(double x, double y, double vx, double vy, int unum,
                           const Vector2D &ball_pos, const Action &action)
    : Vector2D(x, y, vx, vy), _unum(unum), _ball_pos(ball_pos), _action(action) {}
PlayerObject::PlayerObject(int unum, const Action &action)
    : Vector2D(0,0,0,0), _unum(unum), _ball_pos(0,0,0,0), _action(action){}

bool PlayerObject::isKickable() const {
    return (pos().dist(_ball_pos) < 1.5);
}

int PlayerObject::unum() const {
    return _unum;
}

double PlayerObject::distFromBall() const {
    return pos().dist(_ball_pos);
}

const Action& PlayerObject::action() const {
    return _action;
}

std::string PlayerObject::kick() const {
    return _action.kick;
}

std::string PlayerObject::dash() const {
    return _action.dash;
}

std::string PlayerObject::turn() const {
    return _action.turn;
}

std::string PlayerObject::attentionto() const {
    return _action.attentionto;
}

std::string PlayerObject::change_view() const {
    return _action.change_view;
}

std::string PlayerObject::pointto() const {
    return _action.pointto;
}

std::string PlayerObject::turn_neck() const {
    return _action.turn_neck;
}

std::string PlayerObject::say() const {
    return _action.say;
}

std::string PlayerObject::tackle() const {
    return _action.tackle;
}

