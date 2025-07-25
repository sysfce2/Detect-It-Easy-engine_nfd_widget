QT       += concurrent

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialognfdscan.h \
    $$PWD/dialognfdscandirectory.h \
    $$PWD/dialognfdwidgetadvanced.h \
    $$PWD/nfd_widget.h \
    $$PWD/nfdoptionswidget.h \
    $$PWD/nfdwidgetadvanced.h

SOURCES += \
    $$PWD/dialognfdscan.cpp \
    $$PWD/dialognfdscandirectory.cpp \
    $$PWD/dialognfdwidgetadvanced.cpp \
    $$PWD/nfd_widget.cpp \
    $$PWD/nfdoptionswidget.cpp \
    $$PWD/nfdwidgetadvanced.cpp

FORMS += \
    $$PWD/dialognfdscan.ui \
    $$PWD/dialognfdscandirectory.ui \
    $$PWD/dialognfdwidgetadvanced.ui \
    $$PWD/nfd_widget.ui \
    $$PWD/nfdoptionswidget.ui \
    $$PWD/nfdwidgetadvanced.ui

!contains(XCONFIG, specabstract) {
    XCONFIG += specabstract
    include($$PWD/../SpecAbstract/specabstract.pri)
}

!contains(XCONFIG, xshortcuts) {
    XCONFIG += xshortcuts
    include($$PWD/../XShortcuts/xshortcuts.pri)
}

!contains(XCONFIG, dialogtextinfo) {
    XCONFIG += dialogtextinfo
    include($$PWD/../FormatDialogs/dialogtextinfo.pri)
}

!contains(XCONFIG, xdialogprocess) {
    XCONFIG += xdialogprocess
    include($$PWD/../FormatDialogs/xdialogprocess.pri)
}

!contains(XCONFIG, xcomboboxex) {
    XCONFIG += xcomboboxex
    include($$PWD/../Controls/xcomboboxex.pri)
}

!contains(XCONFIG, xtableview) {
    XCONFIG += xtableview
    include($$PWD/../Controls/xtableview.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/nfd_widget.cmake
