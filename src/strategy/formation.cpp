#include "formation.h"

// Kickoff nosso: robô 3 na bola, outros em posição
std::array<RobotTarget, 6> formation_kickoff_us() {
    return {{
        { 0, -3800,    0 },  // goleiro
        { 1, -2000, -800 },  // defensor esquerdo
        { 2, -2000,  800 },  // defensor direito
        { 3,  -200,    0 },  // atacante perto da bola
        { 4,  -800, -600 },  // meia esquerdo
        { 5,  -800,  600 },  // meia direito
    }};
}

// Kickoff deles: todos atrás da linha do meio
std::array<RobotTarget, 6> formation_kickoff_them() {
    return {{
        { 0, -3800,    0 },
        { 1, -2000, -800 },
        { 2, -2000,  800 },
        { 3,  -600,    0 },
        { 4,  -800, -600 },
        { 5,  -800,  600 },
    }};
}

// Falta nossa: robô 3 na bola, outros abertos
std::array<RobotTarget, 6> formation_free_kick_us() {
    return {{
        { 0, -3800,    0 },
        { 1, -2000, -800 },
        { 2, -2000,  800 },
        { 3,  -100,    0 },  // próximo à bola
        { 4,   500, -700 },  // aberto para receber
        { 5,   500,  700 },  // aberto para receber
    }};
}

// Stop: todos recuam e param
std::array<RobotTarget, 6> formation_stop() {
    return {{
        { 0, -3800,    0 },
        { 1, -2500, -800 },
        { 2, -2500,  800 },
        { 3,  -500, -500 },
        { 4,  -500,  500 },
        { 5,   500,    0 },
    }};
}
