PROJECT( license-kcm )

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/license-widget ${LIBLICENSE_INCLUDE_DIR} )

SET(kcmLiblicenseSources kcm_liblicense.cpp ../license-widget/licensechooser.cpp)

KDE4_AUTOMOC( ${kcmLiblicenseSources} )

KDE4_ADD_UI_FILES( kcmLiblicenseSources ../license-widget/license_chooser.ui )
KDE4_ADD_PLUGIN(kcm_liblicense ${kcmLiblicenseSources})

TARGET_LINK_LIBRARIES(kcm_liblicense ${KDE4_KPARTS_LIBS}  ${LIBLICENSE_LDFLAGS} )
 
INSTALL(TARGETS kcm_liblicense  DESTINATION ${PLUGIN_INSTALL_DIR} )
INSTALL( FILES liblicense.desktop  DESTINATION ${SERVICES_INSTALL_DIR}  )
