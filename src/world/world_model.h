#pragma once
#include <string>
#include <optional>
#include <mutex>
#include <libmemcached/memcached.h>

struct BallState {
    float   x, y, vx, vy;
    int64_t timestamp_ms;
};

struct RobotState {
    int     id;
    float   x, y, angle;
    float   vx, vy;
    int64_t timestamp_ms;
};

class WorldModel {
public:
    WorldModel();
    ~WorldModel();

    void set_ball(const BallState& ball);
    void set_robot_blue(const RobotState& robot);
    void set_robot_yellow(const RobotState& robot);
    void set_game_state(const std::string& state);

    std::optional<BallState>  get_ball();
    std::optional<RobotState> get_robot_blue(int id);
    std::optional<RobotState> get_robot_yellow(int id);
    std::string               get_game_state();

private:
    memcached_st* mc_;
    std::mutex    mutex_;

    void        mc_set(const std::string& key, const std::string& value, time_t ttl);
    std::string mc_get(const std::string& key);
};
