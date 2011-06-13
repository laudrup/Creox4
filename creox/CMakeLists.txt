include_directories(
  ${CMAKE_SOURCE_DIR}/creox/effects_gui
  ${CMAKE_SOURCE_DIR}/creox/cr_lib
  ${CMAKE_SOURCE_DIR}/creox/effects_dsp
  ${CMAKE_SOURCE_DIR}/creox/dsp_flow
  ${CMAKE_SOURCE_DIR}/creox/preset
  ${KDE4_INCLUDES}
  ${QT_INCLUDES}
  ${JACK_INCLUDES}
  ${QT_QT3SUPPORT_INCLUDE_DIR}
  )

add_subdirectory(cr_lib)
add_subdirectory(dsp_flow)
add_subdirectory(effects_dsp)
add_subdirectory(effects_gui)
add_subdirectory(pics)
add_subdirectory(preset)

set(creox_SRCS
    privilegesswitcher.cpp
    crthreadeventdispatcher.cpp
    crsplashscreen.cpp
    croptionsdialog.cpp
    crchainview.cpp
    crchainpopupmenu.cpp
    crchainbutton.cpp
    crbutton.cpp
    creox.cpp
    main.cpp)

kde4_add_executable(creox ${creox_SRCS})

target_link_libraries(creox
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
  ${QT_QT3SUPPORT_LIBRARIES}
  ${JACK_LIBRARIES}
  effects_gui
  effects_dsp
  dsp_flow
  cr_lib
  preset)

install(TARGETS creox ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES creoxui.rc DESTINATION ${DATA_INSTALL_DIR}/creox)
