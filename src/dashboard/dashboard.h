#pragma once
#include "world/world_model.h"
#include "strategy/strategy_engine.h"

class Dashboard {
public:
    Dashboard(WorldModel& world, StrategyEngine& strategy);
    void render();

private:
    WorldModel&     world_;
    StrategyEngine& strategy_;

    static constexpr int   FIELD_W = 60;
    static constexpr int   FIELD_H = 26;
    static constexpr float FIELD_X = 4500.0f;
    static constexpr float FIELD_Y = 3000.0f;

    int to_col(float x);
    int to_row(float y);
};
