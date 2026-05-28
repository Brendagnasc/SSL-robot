#include "referee_client.h"
#include <iostream>

using namespace boost::asio;
using udp = ip::udp;

RefereeClient::RefereeClient(io_context& io, WorldModel& world,
                             const std::string& host, unsigned short port)
    : socket_(io), world_(world)
{
    auto endpoint = udp::endpoint(ip::address::from_string("0.0.0.0"), port);
    socket_.open(endpoint.protocol());
    socket_.set_option(udp::socket::reuse_address(true));
    socket_.bind(endpoint);
    socket_.set_option(ip::multicast::join_group(
        ip::address::from_string(host)
    ));
    std::cout << "[RefereeClient] Ouvindo " << host << ":" << port << "\n";
}

void RefereeClient::start() { receive(); }

void RefereeClient::receive() {
    socket_.async_receive_from(
        buffer(buffer_), sender_,
        [this](std::error_code ec, std::size_t bytes) {
            if (!ec && bytes > 0) {
                Referee ref;
                if (ref.ParseFromArray(buffer_.data(), bytes))
                    process(ref);
            }
            receive();
        }
    );
}

void RefereeClient::process(const Referee& ref) {
    std::string state;
    switch (ref.command()) {
        case Referee::HALT:              state = "HALT";    break;
        case Referee::STOP:              state = "STOP";    break;
        case Referee::NORMAL_START:
        case Referee::FORCE_START:       state = "RUNNING"; break;
        case Referee::PREPARE_KICKOFF_BLUE:   state = "KICKOFF_US";   break;
        case Referee::PREPARE_KICKOFF_YELLOW: state = "KICKOFF_THEM"; break;
        case Referee::DIRECT_FREE_BLUE:       state = "FREE_KICK_US"; break;
        case Referee::DIRECT_FREE_YELLOW:     state = "FREE_KICK_THEM"; break;
        case Referee::BALL_PLACEMENT_BLUE:    state = "PLACEMENT_US"; break;
        default:                              state = "STOP";          break;
    }
    world_.set_game_state(state);
    std::cout << "[Referee] Comando: " << state << "\n";
}
