PROJECT( license-properties-plugin )

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/license-widget ${LIBLICENSE_INCLUDE_DIR} )

SET(propsLiblicenseSources licenseprops.cpp ../license-widget/licensechooser.cpp)

KDE4_ADD_UI_FILES( propsLiblicenseSources ../license-widget/license_chooser.ui )

KDE4_ADD_PLUGIN(licenseprops ${propsLiblicenseSources})

TARGET_LINK_LIBRARIES(licenseprops license ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KPARTS_LIBS} ${LIBLICENSE_LDFLAGS} )
 
install(TARGETS licenseprops  DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES licenseprops.desktop  DESTINATION ${SERVICES_INSTALL_DIR}  )
