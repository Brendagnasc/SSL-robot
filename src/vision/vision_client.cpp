#include "vision_client.h"
#include <iostream>
#include <chrono>

using namespace boost::asio;
using udp = ip::udp;

static int64_t now_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

VisionClient::VisionClient(io_context& io, WorldModel& world,
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
    std::cout << "[VisionClient] Ouvindo " << host << ":" << port << "\n";
}

void VisionClient::start() { receive(); }

void VisionClient::receive() {
    socket_.async_receive_from(
        buffer(buffer_), sender_,
        [this](std::error_code ec, std::size_t bytes) {
            if (!ec && bytes > 0) {
                SSL_WrapperPacket pkt;
                if (pkt.ParseFromArray(buffer_.data(), bytes))
                    process(pkt);
            }
            receive();
        }
    );
}

void VisionClient::process(const SSL_WrapperPacket& pkt) {
    if (!pkt.has_detection()) return;
    const auto& det = pkt.detection();

    if (!det.balls().empty()) {
        const auto& b = det.balls(0);
        world_.set_ball({ b.x(), b.y(), 0.f, 0.f, now_ms() });
        std::cout << "[Vision] Bola: (" << b.x() << ", " << b.y() << ")\n";
    }

    for (const auto& r : det.robots_blue())
        world_.set_robot_blue({ r.robot_id(), r.x(), r.y(), r.orientation(), 0, 0, now_ms() });

    for (const auto& r : det.robots_yellow())
        world_.set_robot_yellow({ r.robot_id(), r.x(), r.y(), r.orientation(), 0, 0, now_ms() });
}
