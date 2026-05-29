#include "dashboard.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>

Dashboard::Dashboard(WorldModel& world, StrategyEngine& strategy)
    : world_(world), strategy_(strategy) {}

int Dashboard::to_col(float x) {
    return (int)((x + FIELD_X) / (2 * FIELD_X) * (FIELD_W - 2)) + 1;
}

int Dashboard::to_row(float y) {
    return (int)((-y + FIELD_Y) / (2 * FIELD_Y) * (FIELD_H - 2)) + 1;
}

void Dashboard::render() {
    // Monta o grid do campo
    std::vector<std::string> grid(FIELD_H, std::string(FIELD_W, ' '));

    // Bordas
    for (int c = 0; c < FIELD_W; c++) {
        grid[0][c] = '-';
        grid[FIELD_H-1][c] = '-';
    }
    for (int r = 0; r < FIELD_H; r++) {
        grid[r][0] = '|';
        grid[r][FIELD_W-1] = '|';
    }

    // Linha do meio
    int mid_col = FIELD_W / 2;
    for (int r = 1; r < FIELD_H-1; r++)
        grid[r][mid_col] = ':';

    // Gols
    int gol_top = FIELD_H/2 - 2;
    int gol_bot = FIELD_H/2 + 2;
    for (int r = gol_top; r <= gol_bot; r++) {
        grid[r][0] = '=';
        grid[r][FIELD_W-1] = '=';
    }

    // Bola
    auto ball = world_.get_ball();
    if (ball) {
        int bc = to_col(ball->x);
        int br = to_row(ball->y);
        if (bc > 0 && bc < FIELD_W-1 && br > 0 && br < FIELD_H-1)
            grid[br][bc] = 'o';
    }

    // Robôs azuis
    auto roles = strategy_.get_roles();
    for (auto& role : roles) {
        auto r = world_.get_robot_blue(role.robot_id);
        if (!r) continue;
        int rc = to_col(r->x);
        int rr = to_row(r->y);
        if (rc > 0 && rc < FIELD_W-1 && rr > 0 && rr < FIELD_H-1) {
            char sym = '0' + role.robot_id;
            grid[rr][rc] = sym;
        }
    }

    // Limpa tela e renderiza
    std::cout << "\033[2J\033[H"; // clear screen
    std::cout << "=== SSL Robot Dashboard ===\n";
    std::cout << "Estado: " << world_.get_game_state() << "\n";
    if (ball)
        std::cout << "Bola: (" << std::fixed << std::setprecision(0)
                  << ball->x << ", " << ball->y << ")\n";
    std::cout << "\n";

    for (auto& row : grid)
        std::cout << row << "\n";

    std::cout << "\nLegenda: 0=goleiro 1,2=defensor 3,4=meio 5=atacante o=bola\n";
    std::cout << "Papéis:  ";
    for (auto& role : roles)
        std::cout << "R" << role.robot_id << "=" << role_to_string(role.role) << " ";
    std::cout << "\n";
}
