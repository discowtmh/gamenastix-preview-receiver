// This file is a part of Treadmill project.
// Copyright 2018 Disco WTMH S.A.

#pragma once

#include <QHostAddress>
#include <QNetworkDatagram>
#include <QUdpSocket>
#include <Protocol.h>


class UdpForwarder
{
public:
    std::unique_ptr<QUdpSocket> udpSocket;
    QHostAddress host;
    uint16_t port;

    UdpForwarder()
        : udpSocket(std::make_unique<QUdpSocket>())
        , host(QHostAddress::LocalHost)
        , port(1234)
    {

        udpSocket->connectToHost(QHostAddress::Broadcast, port);
        udpSocket->waitForConnected();
        udpSocket->bind(port, QUdpSocket::ShareAddress);
    }

    void send(Message &message)
    {
        QByteArray forwarderDatagramTyped(reinterpret_cast<const char *>(message.data()), static_cast<int>(message.size()));
        udpSocket->writeDatagram(forwarderDatagramTyped, host, port);
    }
};
