TARGET = flappyurho

LIBS += ../FlappyUrho/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -ldl \
    -lGL

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH += \
    ../FlappyUrho/Urho3D/include \
    ../FlappyUrho/Urho3D/include/Urho3D/ThirdParty \

TEMPLATE = app
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    mastercontrol.cpp \
    weed.cpp \
    global.cpp \
    fish.cpp \
    environment.cpp \
    crown.cpp \
    flappycam.cpp \
    barrier.cpp \
    score3d.cpp

HEADERS += \
    Urho3DAll.h \
    mastercontrol.h \
    weed.h \
    global.h \
    fish.h \
    environment.h \
    crown.h \
    flappycam.h \
    barrier.h \
    score3d.h

OTHER_FILES += \
