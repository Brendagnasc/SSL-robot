#include "robot_comm.h"
#include "ssl_simulation_robot_control.pb.h"
#include <iostream>

using namespace boost::asio;
using udp = ip::udp;

RobotComm::RobotComm(const std::string& host, unsigned short port)
    : socket_(io_)
{
    socket_.open(udp::v4());
    endpoint_ = udp::endpoint(ip::address::from_string(host), port);
    std::cout << "[RobotComm] Enviando para " << host << ":" << port << "\n";
}

RobotComm::~RobotComm() {
    socket_.close();
}

void RobotComm::send(const RobotCmd& cmd) {
    RobotControl control;
    auto* robot_cmd = control.add_robot_commands();

    robot_cmd->set_id(cmd.robot_id);

    // Usa velocidade global (eixo do campo) em vez de local (eixo do robô)
    auto* move  = robot_cmd->mutable_move_command();
    auto* vel   = move->mutable_global_velocity();
    vel->set_x(cmd.vx);
    vel->set_y(cmd.vy);
    vel->set_angular(cmd.vw);

    if (cmd.kick) {
        robot_cmd->set_kick_speed(cmd.kick_power);
        robot_cmd->set_kick_angle(0.0f);
    }

    std::string data;
    control.SerializeToString(&data);
    socket_.send_to(buffer(data), endpoint_);
}
