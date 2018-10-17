#include <QSerialPortInfo>

#include "SerialTesterMainWindow.h"
#include "ui_SerialTesterMainWindow.h"
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
    updateAvailablePortsComboBox(*ui->availablePortsComboBox);
}

SerialTesterMainWindow::~SerialTesterMainWindow()
{
    delete ui;
}

const char * getenv_or(const char * environmentVariableName, const char* defaultValue)
{
    const char * returnValue = getenv(environmentVariableName);
    return returnValue ? returnValue : defaultValue;
}

void SerialTesterMainWindow::updateAvailablePortsComboBox(QComboBox &availablePortsComboBox)
{
    availablePortsComboBox.clear();

    for (QSerialPortInfo port : QSerialPortInfo::availablePorts())
    {
        availablePortsComboBox.addItem(port.portName());
    }
    availablePortsComboBox.addItem(getenv_or("COM_TESTER", "/dev/tnt1"));
}

QSerialPort::BaudRate SerialTesterMainWindow::getBaudRate(int index)
{
    switch (index)
    {
        case 0:
            return QSerialPort::Baud9600;
        case 1:
            return QSerialPort::Baud115200;
        default:
            return QSerialPort::Baud9600;
    }
}

void SerialTesterMainWindow::errorOccurred(QSerialPort::SerialPortError error)
{
    std::cerr << error << std::endl;
    on_connectButton_clicked();
}

void SerialTesterMainWindow::disconnectSerialPort(std::unique_ptr<QSerialPort> &serial)
{
    serial.reset();
}

void SerialTesterMainWindow::connectSerialPort(std::unique_ptr<QSerialPort> &serialPortHandle)
{
    QString port = ui->availablePortsComboBox->currentText();
    QSerialPort::BaudRate baudRate = getBaudRate(ui->baudRateComboBox->currentIndex());
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

void SerialTesterMainWindow::readData()
{
    auto incomingData = serialPortHandle->readAll();
    bool isMessageCompleted = false;
    for (char c : incomingData)
    {
        std::cout << c;
        serialBuffer[serialBufferIndex++] = c;
        if (c == '\n')
        {
            isMessageCompleted = true;
        }
        if (serialBufferIndex == 1024)
        {
            serialBufferIndex = 0;
        }
    }
    if (!isMessageCompleted)
    {
        return;
    }
    incomingData = QByteArray(serialBuffer, serialBufferIndex);
    serialBufferIndex = 0;
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
            break;
        }
    }
}

void SerialTesterMainWindow::resetComPort(
    std::unique_ptr<QSerialPort> &serial)
{
    if (serial)
    {
        disconnectSerialPort(serial);
    }
    else
    {
        connectSerialPort(serial);
    }
}


void SerialTesterMainWindow::on_refreshAvailablePortsButton_clicked()
{
    updateAvailablePortsComboBox(*ui->availablePortsComboBox);
}

void SerialTesterMainWindow::on_connectButton_clicked()
{
    connectSerialPort(serialPortHandle);
}

void SerialTesterMainWindow::on_resetReceivingButton_clicked()
{
    ui->receivingTextEdit->clear();
}

void SerialTesterMainWindow::sendBufferContentWithReset()
{
    auto data = ui->sendingTextEdit->toPlainText().toStdString() + '\n';
    ui->sendingTextEdit->clear();
    std::cout << "[" << data << "]" << std::endl;
    serialPortHandle->write(reinterpret_cast<const char *>(data.data()), data.size());
}

void SerialTesterMainWindow::on_sendButton_clicked()
{
    sendBufferContentWithReset();
}

void SerialTesterMainWindow::on_sendRequestModelMessageButton_clicked()
{
    static const char *requestModelMessage = "R";
    serialPortHandle->write(requestModelMessage, 1);
}

void SerialTesterMainWindow::on_sendCalibrateNPoseMessageButton_clicked()
{
    static const char *calibrateNPoseMessage = "N";
    serialPortHandle->write(calibrateNPoseMessage, 1);
}

void SerialTesterMainWindow::on_sendCalibrateSPoseMessageButton_clicked()
{
    static const char *calibrateSPoseMessage = "S";
    serialPortHandle->write(calibrateSPoseMessage, 1);
}
