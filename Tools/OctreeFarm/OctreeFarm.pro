QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OctreeFarm
TEMPLATE = app
LIBS += -L$$(GLEW_HOME)/lib -lglew32
WIN32: LIBS += -lopengl32

INCLUDEPATH += "../../ThirdParty" $$(GLEW_HOME)/include

SOURCES += main.cpp\
    MainWindow.cpp \
    Octree.cpp \
    Camera.cpp \
    Viewport.cpp \
    Properties.cpp \
    Source.cpp \
    ../../Source/Core/Utils.cpp \
    ../../Source/Core/Object.cpp

HEADERS  += MainWindow.h \
    Octree.h \
    Camera.h \
    Viewport.h \
    Properties.h \
    ../../Source/Core/Common.h \
    Source.h \
    ../../Source/Core/Utils.h \
    ../../Source/Core/Object.h
