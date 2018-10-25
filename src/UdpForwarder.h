// This file is a part of Treadmill project.
// Copyright 2018 Disco WTMH S.A.

#pragma once

#include <Protocol.h>
#include <QHostAddress>
#include <QNetworkDatagram>
#include <QUdpSocket>


class UdpForwarder
{
public:
    std::unique_ptr<QUdpSocket> udpSocket;
    QHostAddress host;
    uint16_t port;

    UdpForwarder(QHostAddress host = QHostAddress::LocalHost, uint16_t port = 1234)
            : udpSocket(std::make_unique<QUdpSocket>())
            , host(host)
            , port(port)
    {
        udpSocket->connectToHost(QHostAddress::Broadcast, port);
        udpSocket->waitForConnected();
    }

    void send(Message &message)
    {
        QByteArray forwarderDatagramTyped(reinterpret_cast<const char *>(message.data()), static_cast<int>(message.size()));
        udpSocket->writeDatagram(forwarderDatagramTyped, host, port);
    }
};
