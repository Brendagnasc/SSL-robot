#pragma once
#include "skill.h"

class MoveToPosition : public Skill {
public:
    MoveToPosition(float target_x, float target_y, float target_angle = 0.0f);

    RobotCommand execute(int robot_id, WorldModel& world) override;
    bool is_done() const override;

    void set_target(float x, float y, float angle = 0.0f);

private:
    float target_x_;
    float target_y_;
    float target_angle_;
    bool  done_;

    static constexpr float ARRIVAL_THRESHOLD = 50.0f;  // mm
    static constexpr float MAX_SPEED         = 1.5f;   // m/s
    static constexpr float KP                = 0.003f; // proporcional
};
