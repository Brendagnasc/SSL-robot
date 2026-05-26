#pragma once
#include "world/world_model.h"

struct RobotCmd {
    int   robot_id;
    float vx;
    float vy;
    float vw;
    bool  kick;
    float kick_power;
};

class Skill {
public:
    virtual ~Skill() = default;
    virtual RobotCmd execute(int robot_id, WorldModel& world) = 0;
    virtual bool is_done() const { return false; }
};
