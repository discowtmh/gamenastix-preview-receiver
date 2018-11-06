#include <QSerialPortInfo>

#include "SerialTesterMainWindow.h"
#include "ui_SerialTesterMainWindow.h"
#include <Protocol.h>
#include <SystemClock.h>
#include <iostream>

#include <deepModel/Treadmill.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <Protocol.h>
#include <SystemClock.h>
#include <iostream>
#include <memory>

char serialBuffer[1024];
int serialBufferIndex = 0;


SerialTesterMainWindow::SerialTesterMainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::SerialTesterMainWindow)
{
    ui->setupUi(this);
    updateAvailablePortsComboBox(*ui->availablePortsComboBox_FromSupervisor);
}

SerialTesterMainWindow::~SerialTesterMainWindow()
{
    delete ui;
}

const char *getenv_or(const char *environmentVariableName, const char *defaultValue)
{
    const char *returnValue = getenv(environmentVariableName);
    return returnValue ? returnValue : defaultValue;
}

void SerialTesterMainWindow::updateAvailablePortsComboBox(QComboBox &availablePortsComboBox)
{
    availablePortsComboBox.clear();

    for (QSerialPortInfo port : QSerialPortInfo::availablePorts())
    {
        availablePortsComboBox.addItem(port.portName());
    }
}

QSerialPort::BaudRate SerialTesterMainWindow::getBaudRate(int index)
{
    switch (index)
    {
        case 0:
            return QSerialPort::Baud115200;
        default:
            return QSerialPort::Baud115200;
    }
}

void SerialTesterMainWindow::errorOccurred(QSerialPort::SerialPortError error)
{
    std::cerr << error << std::endl;
}

void SerialTesterMainWindow::disconnectSerialPort(std::unique_ptr<QSerialPort> &serial)
{
    serial.reset();
}

void SerialTesterMainWindow::connectSerialPort(std::unique_ptr<QSerialPort> &serialPortHandle,
                                               QComboBox &comboBoxPort,
                                               QComboBox &comboBoxBaud)
{
    QString port = comboBoxPort.currentText();
    QSerialPort::BaudRate baudRate = getBaudRate(comboBoxBaud.currentIndex());
    connectSerialPort(serialPortHandle, port, baudRate);
}

void SerialTesterMainWindow::connectSerialPort(
    std::unique_ptr<QSerialPort> &serial,
    QString port,
    QSerialPort::BaudRate baudRate)
{
    serial = std::make_unique<QSerialPort>(this);

    serial->setPortName(port);
    serial->setBaudRate(baudRate);
    serial->open(QIODevice::ReadWrite);

    connect(serial.get(), &QSerialPort::readyRead, this, &SerialTesterMainWindow::readData);
}


void SerialTesterMainWindow::handleFrame(Message &message)
{
    if (udpForwarder)
    {
        udpForwarder->send(message);
    }
}

void SerialTesterMainWindow::readData()
{
    auto incomingData = serialPortHandle_FromSupervisor->readAll();
    ui->receivingTextEdit->append(incomingData);
    Message message(incomingData.begin(), incomingData.end());
    switch (Matcher::match(message))
    {
        case MessageType::ResponseModel:
        {
            ui->responseModelMessage->setText(incomingData);
            ui->responseModelTimestamp->setText(QString::number(SystemClock::millis()));
            break;
        }
        case MessageType::Frame:
        {
            ui->frameMessage->setText(incomingData);
            ui->frameTimestamp->setText(QString::number(SystemClock::millis()));
            handleFrame(message);
            break;
        }
    }
}

void SerialTesterMainWindow::resetComPort(
    std::unique_ptr<QSerialPort> &serial,
    QComboBox &comboBoxPort,
    QComboBox &comboBoxBaud,
    QPushButton &button,
    QString connectText,
    QString disconnectText)
{
    if (serial)
    {
        disconnectSerialPort(serial);
        button.setText(connectText);
    }
    else
    {
        connectSerialPort(serial, comboBoxPort, comboBoxBaud);
        button.setText(disconnectText);
    }
}

void SerialTesterMainWindow::on_resetReceivingButton_clicked()
{
    ui->receivingTextEdit->clear();
}

void SerialTesterMainWindow::sendBufferContentWithReset()
{
    auto data = ui->sendingTextEdit->toPlainText();
    ui->sendingTextEdit->clear();
    serialPortHandle_FromSupervisor->write(reinterpret_cast<const char *>(data.data()), data.size());
}

void SerialTesterMainWindow::on_sendButton_clicked()
{
    sendBufferContentWithReset();
}

void SerialTesterMainWindow::on_sendRequestModelMessageButton_clicked()
{
    static const char *requestModelMessage = "R";
    serialPortHandle_FromSupervisor->write(requestModelMessage, 1);
}

void SerialTesterMainWindow::on_sendCalibrateNPoseMessageButton_clicked()
{
    static const char *calibrateNPoseMessage = "N";
    serialPortHandle_FromSupervisor->write(calibrateNPoseMessage, 1);
}

void SerialTesterMainWindow::on_sendCalibrateSPoseMessageButton_clicked()
{
    static const char *calibrateSPoseMessage = "S";
    serialPortHandle_FromSupervisor->write(calibrateSPoseMessage, 1);
}

void SerialTesterMainWindow::on_refreshAvailablePortsButton_FromSupervisor_clicked()
{
    updateAvailablePortsComboBox(*ui->availablePortsComboBox_FromSupervisor);
}

void SerialTesterMainWindow::on_connectButton_FromSupervisor_clicked()
{
    resetComPort(serialPortHandle_FromSupervisor, *ui->availablePortsComboBox_FromSupervisor, *ui->baudRateComboBox_FromSupervisor, *ui->connectButton_FromSupervisor, "Connect Supervisor", "Disconnect Supervisor");
}

void SerialTesterMainWindow::on_connectButton_ToPreview_clicked()
{
    if (udpForwarder)
    {
        ui->connectButton_ToPreview->setText("Forward UDP Packet");
        udpForwarder.reset();
    }
    else
    {
        ui->connectButton_ToPreview->setText("Stop Forwarding");
        udpForwarder = std::make_unique<UdpForwarder>(QHostAddress(ui->previewHost->text()), ui->previewPort->text().toInt());
    }

}
