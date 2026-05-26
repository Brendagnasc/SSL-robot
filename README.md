# SSL Robot — RoboCup Small Size League

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Subleague](https://img.shields.io/badge/RoboCup-SSL%20Division%20A-green.svg)](https://ssl.robocup.org/)

Projeto de inteligência artificial e controle de futebol de robôs desenvolvido para a **RoboCup Small Size League (SSL)**, especificamente para a **Division A (6v6)**. O sistema é responsável por processar os dados geométricos do campo e de arbitragem em tempo real, gerando o modelo de mundo, definindo táticas e enviando comandos de movimentação para os robôs.

---

## 🛠️ Stack Tecnológica

* **Linguagem Principal:** C++17 (foco em alto desempenho e baixa latência)
* **Simulador:** [grSim](https://github.com/RoboCup-SSL/grSim)
* **Gerenciamento de Estado:** [Memcached](https://memcached.org/) (utilizado como memória compartilhada e cache para cálculos geométricos pesados)
* **Protocolos de Rede:** SSL-Vision e SSL-Refbox via Google Protocol Buffers (Protobuf) encapsulados em UDP multicast.

---

## 📂 Estrutura do Projeto

```text
ssl-robot/
├── proto/           # Arquivos .proto oficiais da SSL (Vision, Referee e grSim)
├── src/
│   ├── vision/      # Cliente receptor e parser dos pacotes do SSL-Vision
│   ├── referee/     # Cliente receptor dos comandos do juiz (Refbox)
│   ├── world/       # Modelo de Mundo e persistência/sincronização via Memcached
│   ├── strategy/    # Motor de táticas, papéis, jogadas e tomada de decisão de alto nível
│   ├── skills/      # Comportamentos individuais dos robôs (ex: GoToPoint, Kick, Intercept)
│   └── comm/        # Módulo de envio de comandos de baixa latência (rádio ou simulador)
└── tests/           # Testes unitários e de integração
