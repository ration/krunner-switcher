cmake_minimum_required(VERSION 3.15)

project(switcher)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)

set(QT_MAJOR_VERSION "6")
set(QT_MIN_VERSION "6.4.0")
set(KF_MAJOR_VERSION "6")
set(KF6_MIN_VERSION "6.4.0")

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)
include(ECMQtDeclareLoggingCategory)
include(KDEClangFormat)
include(KDEGitCommitHooks)


find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION}
  CONFIG REQUIRED
  COMPONENTS
  Widgets)
find_package(KF${KF_MAJOR_VERSION} ${KF6_MIN_VERSION}
  REQUIRED COMPONENTS
  Runner
  I18n
  WindowSystem
  # Pulled in by Plasma
  Config)

add_subdirectory(src)



feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

# add clang-format target for all our real source files
file(GLOB ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
