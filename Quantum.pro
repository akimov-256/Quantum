QT       += core gui
QT += quick qml quickcontrols2 network
QT += svg sql
LIBS += -lShell32

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = qml/assets/icons/icon.ico

SOURCES += \
    src/backend/backend.cpp \
    src/backend/downloadmodel.cpp \
    src/backend/filenamehandler.cpp \
    src/core/databasemanager.cpp \
    src/core/downloader.cpp \
    src/core/downloadworker.cpp \
    src/core/main.cpp

HEADERS += \
    src/backend/backend.h \
    src/backend/downloadmodel.h \
    src/backend/filenamehandler.h \
    src/core/databasemanager.h \
    src/core/downloader.h \
    src/models/downloadcategories.h \
    src/models/downloadstatus.h \
    src/core/downloadworker.h

FORMS +=

TRANSLATIONS += \
    Quantum_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    web-integration/chrome/background.js \
    web-integration/chrome/content.js \
    web-integration/chrome/manifest.json \
    web-integration/chrome/popup.html \
    web-integration/chrome/popup.js
