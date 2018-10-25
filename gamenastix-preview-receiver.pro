QT       += core gui widgets serialport network

TARGET = gamenastix-preview-receiver
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += src/
INCLUDEPATH += deps/gamenastix-protocol/include/
INCLUDEPATH += deps/gamenastix-biomodel/include/
INCLUDEPATH += deps/glm/

CONFIG += c++14

QMAKE_CXXFLAGS += -DGLM_ENABLE_EXPERIMENTAL

SOURCES +=                                                      \
    deps/gamenastix-biomodel/include/Frame.cpp                  \
    deps/gamenastix-biomodel/include/Mode.cpp                   \
    deps/gamenastix-biomodel/include/Parser.cpp                 \
    deps/gamenastix-biomodel/include/Part.cpp                   \
    deps/gamenastix-biomodel/include/Type.cpp                   \
    src/SerialTesterMainWindow.cpp                              \
    src/main.cpp                                                \


HEADERS +=                                                      \
    deps/gamenastix-biomodel/include/Frame.h                    \
    deps/gamenastix-biomodel/include/Mode.h                     \
    deps/gamenastix-biomodel/include/Node.h                     \
    deps/gamenastix-biomodel/include/Parser.h                   \
    deps/gamenastix-biomodel/include/Part.h                     \
    deps/gamenastix-biomodel/include/Type.h                     \
    deps/gamenastix-biomodel/include/deepModel/Bio4.h           \
    deps/gamenastix-biomodel/include/deepModel/Medium.h         \
    deps/gamenastix-biomodel/include/deepModel/Model.h          \
    deps/gamenastix-biomodel/include/deepModel/Treadmill.h      \
    deps/gamenastix-protocol/include/Protocol.h                 \
    src/SerialTesterMainWindow.h                                \
    src/SystemClock.h                                           \
    src/UdpForwarder.h                                          \


FORMS +=                                                        \
    src/SerialTesterMainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
