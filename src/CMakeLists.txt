add_definitions(-DTRANSLATION_DOMAIN=\"plasma_runner_org.kde.switcher\")

set(switcher_SRCS switcher.cpp)
ecm_qt_declare_logging_category(switcher_SRCS
  HEADER switcher_debug.h
  IDENTIFIER KRUNNER_SWITCHER
  CATEGORY_NAME kf.krunner.runner.switcher
  DESCRIPTION "KRunner Switcher Runner"
  EXPORT KRUNNER_SWITCHER
)

add_library(krunner_switcher MODULE ${switcher_SRCS})
target_link_libraries(krunner_switcher
  Qt${QT_MAJOR_VERSION}::Widgets
  KF${KF_MAJOR_VERSION}::WindowSystem
  KF${KF_MAJOR_VERSION}::I18n
  KF${KF_MAJOR_VERSION}::Runner)



install(TARGETS krunner_switcher DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/kf${KF_MAJOR_VERSION}/krunner)
ecm_qt_install_logging_categories(
    EXPORT KRUNNER_SWITCHER
    FILE krunner_switcher.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
