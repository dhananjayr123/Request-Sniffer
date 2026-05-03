#include <iostream>
#include <string>

#include <sys/socket.h>
#include <linux/if_packet.h>

#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

constexpr int ETHERNET_HEADER_SIZE = 14;
constexpr uint16_t IPV4_ETHERTYPE = 0x0800;
constexpr uint16_t TARGET_PORT = 8080;

bool running = true;

void stop(int)
{
    running = false;
}

struct EthernetHeader
{
    unsigned char destination[6];
    unsigned char source[6];
    uint16_t type;
};

bool isIPv4(EthernetHeader *ethernet)
{
    return ntohs(ethernet->type) == IPV4_ETHERTYPE;
}

bool isTCP(iphdr *ip)
{
    return ip->protocol == IPPROTO_TCP;
}

bool isTargetPort(tcphdr *tcp)
{
    uint16_t sourcePort =
        ntohs(tcp->source);

    uint16_t destinationPort =
        ntohs(tcp->dest);

    return sourcePort == TARGET_PORT ||
           destinationPort == TARGET_PORT;
}

std::string extractHTTP(
    unsigned char *packet,
    int packetSize)
{
    auto *ip =
        reinterpret_cast<iphdr *>(
            packet + ETHERNET_HEADER_SIZE);

    int ipHeaderLength =
        ip->ihl * 4;

    auto *tcp =
        reinterpret_cast<tcphdr *>(
            packet +
            ETHERNET_HEADER_SIZE +
            ipHeaderLength);

    int tcpHeaderLength =
        tcp->doff * 4;

    if (!isTargetPort(tcp))
    {
        return "";
    }

    int payloadLength =
        packetSize -
        ETHERNET_HEADER_SIZE -
        ipHeaderLength -
        tcpHeaderLength;

    if (payloadLength <= 0)
    {
        return "";
    }

    unsigned char *payload =
        packet +
        ETHERNET_HEADER_SIZE +
        ipHeaderLength +
        tcpHeaderLength;

    return std::string(
        reinterpret_cast<char *>(payload),
        payloadLength);
}

int main()
{
    signal(SIGINT, stop);

    int socketFd =
        socket(
            AF_PACKET,
            SOCK_RAW,
            htons(ETH_P_ALL));

    if (socketFd < 0)
    {
        perror("socket");
        return 1;
    }

    unsigned char buffer[65536];

    std::cout
        << "HTTP Sniffer started...\n";

    while (running)
    {
        int bytesRead =
            recvfrom(
                socketFd,
                buffer,
                sizeof(buffer),
                0,
                nullptr,
                nullptr);

        if (bytesRead < 0)
        {
            perror("recvfrom");
            break;
        }

        auto *ethernet =
            reinterpret_cast<EthernetHeader *>(
                buffer);

        if (!isIPv4(ethernet))
        {
            continue;
        }

        auto *ip =
            reinterpret_cast<iphdr *>(
                buffer + ETHERNET_HEADER_SIZE);

        if (!isTCP(ip))
        {
            continue;
        }

        std::string request =
            extractHTTP(
                buffer,
                bytesRead);

        if (request.empty())
        {
            continue;
        }

        if (request.find("Host: localhost:8080") == std::string::npos)
        {
            continue;
        }

        if (request.find("POST ") != std::string::npos)
            std::cout
                << "\n==============================\n"
                << request
                << "\n";
    }

    close(socketFd);

    std::cout
        << "\nSniffer stopped.\n";
}