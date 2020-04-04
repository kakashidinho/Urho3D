TARGET = hexon

LIBS += \
    $$PWD/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -ldl \
    -lGL

QMAKE_CXXFLAGS += -std=c++11 -O2

INCLUDEPATH += \
    Urho3D/include \
    Urho3D/include/Urho3D/ThirdParty \

TEMPLATE = app
CONFIG -= app_bundle
CONFIG -= qt

# From http://stackoverflow.com/questions/35772562/inlining-failed-in-call-to-always-inline-m128i-mm-cvtepu8-epi32-m128i-t
#QMAKE_CXX_FLAGS += msse4.2 -mavx
#QMAKE_CFLAGS+=-msse4.2
#QMAKE_CFLAGS+="-march=native -O3"
#QMAKE_CFLAGS    += -msse4.2 -O -g
#QMAKE_CXX_FLAGS += -msse3
#QMAKE_CFLAGS   += -mpopcnt
#QMAKE_CXX_FLAGS += -mpopcnt
#QMAKE_CFLAGS += -march=nehalem
#QMAKE_CXX_FLAGS += -march=nehalem

SOURCES += \
    apple.cpp \
    bubble.cpp \
    bullet.cpp \
    chaoball.cpp \
    chaoflash.cpp \
    chaomine.cpp \
    chaozap.cpp \
    controllable.cpp \
    door.cpp \
    effect.cpp \
    effectmaster.cpp \
    enemy.cpp \
    explosion.cpp \
    flash.cpp \
    heart.cpp \
    hexocam.cpp \
    hitfx.cpp \
    inputmaster.cpp \
    line.cpp \
    luckey.cpp \
    mastercontrol.cpp \
    muzzle.cpp \
    pickup.cpp \
    player.cpp \
    razor.cpp \
    sceneobject.cpp \
    seeker.cpp \
    spawnmaster.cpp \
    spire.cpp \
    splatterpillar.cpp \
    TailGenerator.cpp \
    tile.cpp \
    pilot.cpp \
    phaser.cpp \
    arena.cpp \
    lobby.cpp \
    highest.cpp \
    ship.cpp \
    gui3d.cpp \
    panel.cpp \
    coin.cpp \
    effectinstance.cpp \
    mason.cpp \
    brick.cpp \
    soundeffect.cpp \
    settings.cpp \
    mirage.cpp \
    animatedbillboardset.cpp \
    baphomech.cpp \
    coinpump.cpp

HEADERS += \
    apple.h \
    bubble.h \
    bullet.h \
    chaoball.h \
    chaoflash.h \
    chaomine.h \
    chaozap.h \
    controllable.h \
    door.h \
    effect.h \
    effectmaster.h \
    enemy.h \
    explosion.h \
    flash.h \
    heart.h \
    hexocam.h \
    hitfx.h \
    inputmaster.h \
    line.h \
    luckey.h \
    mastercontrol.h \
    muzzle.h \
    pickup.h \
    player.h \
    razor.h \
    sceneobject.h \
    seeker.h \
    spawnmaster.h \
    spire.h \
    splatterpillar.h \
    TailGenerator.h \
    tile.h \
    pilot.h \
    phaser.h \
    arena.h \
    lobby.h \
    highest.h \
    ship.h \
    hexonevents.h \
    gui3d.h \
    panel.h \
    coin.h \
    effectinstance.h \
    mason.h \
    brick.h \
    soundeffect.h \
    settings.h \
    mirage.h \
    animatedbillboardset.h \
    baphomech.h \
    coinpump.h

OTHER_FILES += \
    Docs/Todo.md \
    Resources/Settings.xml

DISTFILES += \
    LICENSE_TEMPLATE

unix {
    isEmpty(DATADIR) {
        DATADIR = ~/.local/share
    }
    DEFINES += DATADIR=\\\"$${DATADIR}/hexon\\\"

    target.path = /usr/games/
    INSTALLS += target

    resources.path = $$DATADIR/luckey/hexon/
    resources.files = Resources/*
    INSTALLS += resources

    icon.path = $$DATADIR/icons/
    icon.files = hexon.svg
    INSTALLS += icon

    desktop.path = $$DATADIR/applications/
    desktop.files = hexon.desktop
    INSTALLS += desktop
}
