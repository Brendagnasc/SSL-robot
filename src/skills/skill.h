#pragma once
#include "world/world_model.h"

// Comando enviado para o robô
struct RobotCommand {
    int   robot_id;
    float vx;       // velocidade em x (m/s)
    float vy;       // velocidade em y (m/s)
    float vw;       // velocidade angular (rad/s)
    bool  kick;     // chutar?
    float kick_power;
};

// Interface base de todas as skills
class Skill {
public:
    virtual ~Skill() = default;
    virtual RobotCommand execute(int robot_id, WorldModel& world) = 0;
    virtual bool is_done() const { return false; }
};
