add_definitions(-DTRANSLATION_DOMAIN=\"plasma_runner_org.kde.switcher\")

set(switcher_SRCS switcher.cpp)

add_library(krunner_switcher MODULE ${switcher_SRCS})
target_link_libraries(krunner_switcher KF5::Runner KF5::I18n)

install(TARGETS krunner_switcher DESTINATION ${KDE_INSTALL_PLUGINDIR})

install(FILES plasma-runner-switcher.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
