# You can make this with
# qmake seam-carving-gui.pro

TEMPLATE = app
TARGET = SeamCarvingGui
RC_FILE = seam-carving-gui.rc
CONFIG += release
QT += widgets printsupport core gui

#Because the c files have c++ like sytax
!win32:QMAKE_CC = g++ 

macx{
  CONFIG += x86 ppc
  ICON = g.icns
}

# Incluce CAIR as backend
include(cair/cair.pri)	

# Input
HEADERS += mainwindow.h \  
    statebox.h \
    cornergrabber.h \
    ImageScene.h \
    data.h \
    tools.h \
    point.h \
    tree.h

FORMS += resizewidget.ui

SOURCES += main.cpp \
           mainwindow.cpp \   
    statebox.cpp \
    cornergrabber.cpp \
    data.cpp \
    tools.cpp \
    point.cpp
