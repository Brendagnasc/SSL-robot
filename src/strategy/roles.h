#pragma once
#include <string>

enum class Role {
    GOALKEEPER,
    DEFENDER,
    MIDFIELDER,
    ATTACKER
};

inline std::string role_to_string(Role r) {
    switch (r) {
        case Role::GOALKEEPER:  return "goalkeeper";
        case Role::DEFENDER:    return "defender";
        case Role::MIDFIELDER:  return "midfielder";
        case Role::ATTACKER:    return "attacker";
        default:                return "unknown";
    }
}

struct RobotRole {
    int  robot_id;
    Role role;
};
