#pragma once

#include <QComboBox>
#include <QMainWindow>
#include <QSerialPort>
#include <QPushButton>

#include <Protocol.h>
#include "UdpForwarder.h"

#include <glm/glm.hpp>

#include <memory>

namespace Ui {
class SerialTesterMainWindow;
}

class SerialTesterMainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit SerialTesterMainWindow(QWidget *parent = nullptr);
  ~SerialTesterMainWindow();

private slots:
  void on_resetReceivingButton_clicked();

  void on_sendButton_clicked();

  void on_sendRequestModelMessageButton_clicked();

  void on_sendCalibrateNPoseMessageButton_clicked();

  void on_sendCalibrateSPoseMessageButton_clicked();

  void on_refreshAvailablePortsButton_FromSupervisor_clicked();

  void on_connectButton_FromSupervisor_clicked();

  void on_connectButton_ToPreview_clicked();

private:
  void connectSerialPort(std::unique_ptr<QSerialPort> &serialPortHandle,
                         QComboBox &comboBoxPort,
                         QComboBox &comboBoxBaud);
  void connectSerialPort(std::unique_ptr<QSerialPort> &serialPortHandle,
                         QString port,
                         QSerialPort::BaudRate baudRate);
  void disconnectSerialPort(std::unique_ptr<QSerialPort> &serialPortHandle);
  void updateAvailablePortsComboBox(QComboBox &comPortComboBox);
  QSerialPort::BaudRate getBaudRate(int index);
  void errorOccurred(QSerialPort::SerialPortError error);
  void readData();
  void resetComPort(
          std::unique_ptr<QSerialPort> &serial,
          QComboBox &comboBoxPort,
          QComboBox &comboBoxBaud,
          QPushButton &button,
          QString connectText,
          QString disconnectText);
  void sendBufferContentWithReset();
  void handleFrame(Message& message);

private:
  Ui::SerialTesterMainWindow *ui;
  std::unique_ptr<QSerialPort> serialPortHandle_FromSupervisor;

  std::unique_ptr<UdpForwarder> udpForwarder;
};

