# qca qmake profile

TEMPLATE = lib
CONFIG += qt thread release
TARGET = qca

MOC_DIR        = .moc
OBJECTS_DIR    = .obj
UI_DIR         = .ui

VER_MAJ = 2
VER_MIN = 0

# make DLL
win32:{
	CONFIG += dll
	DEFINES += QCA_MAKEDLL
}

QCA_CPP = src
INCLUDEPATH += $$QCA_CPP

# botantools
include(src/botantools/botantools.pri)

HEADERS += \
	$$QCA_CPP/qca.h \
	$$QCA_CPP/qcaprovider.h \
	$$QCA_CPP/qca_plugin.h \
	$$QCA_CPP/qca_systemstore.h

SOURCES += \
	$$QCA_CPP/qca.cpp \
	$$QCA_CPP/qca_plugin.cpp \
	$$QCA_CPP/qca_tools.cpp \
	$$QCA_CPP/qca_basic.cpp \
	$$QCA_CPP/qca_textfilter.cpp \
	$$QCA_CPP/qca_publickey.cpp \
	$$QCA_CPP/qca_cert.cpp \
	$$QCA_CPP/qca_securelayer.cpp \
	$$QCA_CPP/qca_default.cpp

#DEFINES += QCA_NO_SYSTEMSTORE

# debian cert store
DEFINES += QCA_SYSTEMSTORE_PATH='"/etc/ssl/certs/ca-certificates.crt"'
SOURCES += $$QCA_CPP/qca_systemstore_flatfile.cpp

include(conf.pri)
include(extra.pri)

