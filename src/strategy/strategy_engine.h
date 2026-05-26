#pragma once
#include <vector>
#include <array>
#include "roles.h"
#include "world/world_model.h"

class StrategyEngine {
public:
    explicit StrategyEngine(WorldModel& world);

    // Roda um ciclo de decisão tática
    void update();

    // Retorna o papel atual de cada robô
    std::array<RobotRole, 6> get_roles() const;

private:
    WorldModel& world_;
    std::array<RobotRole, 6> roles_;

    // Atribui papéis baseado no estado do jogo
    void assign_roles();

    // Calcula distância entre dois pontos
    float distance(float x1, float y1, float x2, float y2);

    // Encontra o robô mais próximo da bola
    int closest_robot_to_ball();
};
