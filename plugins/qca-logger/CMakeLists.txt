# qca-logger

SET(QCA_LOGGER_SOURCES qca-logger.cpp)
MY_AUTOMOC( QCA_LOGGER_SOURCES )

ADD_LIBRARY(qca-logger MODULE ${QCA_LOGGER_SOURCES})

ADD_DEFINITIONS(${LOGGERH_DEFINITIONS})
INCLUDE_DIRECTORIES(${LOGGERH_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(qca-logger ${QT_QTCORE_LIBRARY})
TARGET_LINK_LIBRARIES(qca-logger qca)

INSTALL(TARGETS qca-logger
        LIBRARY DESTINATION ${qca_PLUGINSDIR}
        ARCHIVE DESTINATION ${qca_PLUGINSDIR}
        RUNTIME DESTINATION ${qca_PLUGINSDIR}
)
