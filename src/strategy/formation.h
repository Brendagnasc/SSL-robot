#pragma once
#include <array>
#include <string>

struct RobotTarget {
    int   robot_id;
    float x, y;
};

// Retorna as posições alvo para cada formação
std::array<RobotTarget, 6> formation_kickoff_us();
std::array<RobotTarget, 6> formation_kickoff_them();
std::array<RobotTarget, 6> formation_free_kick_us();
std::array<RobotTarget, 6> formation_stop();
