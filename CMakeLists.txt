cmake_minimum_required(VERSION 3.19)
project(CPUSchedulingSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(CPUSchedulingSimulator
    WIN32 MACOSX_BUNDLE
    main.cpp

    gui/mainwindow.cpp
    gui/mainwindow.h

    gui/Theme.h
    gui/Theme.cpp
    gui/Sidebar.h
    gui/Sidebar.cpp
    gui/StatCard.h
    gui/StatCard.cpp
    gui/GanttChartWidget.h
    gui/GanttChartWidget.cpp
    gui/GanttChartPanel.h
    gui/GanttChartPanel.cpp
    gui/ProcessDialog.h
    gui/ProcessDialog.cpp

    core/Process.h
    core/GanttSegment.h
    core/ScheduleResult.h
    core/ScheduleResult.cpp

    algorithms/SchedulingAlgorithm.h
    algorithms/FCFSScheduler.h
    algorithms/FCFSScheduler.cpp
    algorithms/SJFScheduler.h
    algorithms/SJFScheduler.cpp
    algorithms/SRTFScheduler.h
    algorithms/SRTFScheduler.cpp
)

target_include_directories(CPUSchedulingSimulator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CPUSchedulingSimulator
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS CPUSchedulingSimulator
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET CPUSchedulingSimulator
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
