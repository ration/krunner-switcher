add_definitions(-DTRANSLATION_DOMAIN=\"plasma_runner_org.kde.switcher\")

set(switcher_SRCS switcher.cpp)

find_package(XCB MODULE REQUIRED COMPONENTS XCB RANDR)

add_library(krunner_switcher MODULE ${switcher_SRCS})
target_link_libraries(krunner_switcher Qt5::Widgets KF5::WindowSystem KF5::I18n KF5::Runner)


configure_file(config-switcher.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-switcher.h)

target_link_libraries(krunner_switcher ${X11_LIBRARIES} XCB::XCB )
target_link_libraries(krunner_switcher Qt5::X11Extras)


install(TARGETS krunner_switcher DESTINATION ${KDE_INSTALL_PLUGINDIR})

install(FILES plasma-runner-switcher.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
