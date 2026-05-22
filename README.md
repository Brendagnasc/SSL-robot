# SSL Robot — RoboCup Small Size League

Projeto de futebol de robôs para a RoboCup Small Size League (SSL), Division A (6v6).

## Stack
- **Linguagem:** C++17
- **Simulador:** grSim
- **Memória de jogo:** Memcached
- **Protocolo:** SSL Vision (protobuf + UDP)

## Estrutura
\`\`\`
ssl-robot/
├── proto/        ← Protobufs oficiais do SSL
├── src/
│   ├── vision/   ← Vision Client (UDP receiver)
│   ├── referee/  ← Referee Client
│   ├── world/    ← World Model + Memcached
│   ├── strategy/ ← Motor de táticas
│   ├── skills/   ← Comportamentos dos robôs
│   └── comm/     ← Comunicação por rádio
└── tests/
\`\`\`

## Como compilar
\`\`\`bash
mkdir build && cd build
cmake ..
make -j$(nproc)
\`\`\`
