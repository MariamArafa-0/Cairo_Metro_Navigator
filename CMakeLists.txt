#cmake_minimum_required(VERSION 3.20)
#project(metro)
#
#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
## Enable Qt's automatic tools
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON) # Automatically processes .qrc files
#set(CMAKE_AUTOUIC ON)
#
#find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
#
#add_executable(metro
#        main.cpp
#        MainWindow.cpp
#        MainWindow.h
#        MetroSystem.cpp
#        MetroSystem.h
#        MetroUkkonen.cpp
#        MetroUkkonen.h
#        resources.qrc  # <--- Added this to include your logo and other resources
#)
#
#target_link_libraries(metro PRIVATE
#        Qt6::Core
#        Qt6::Gui
#        Qt6::Widgets
#)
#
## Copy the data file to the build folder so the app can find it
#file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/metro_data.txt"
#        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
#
#if(WIN32)
#    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
#    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
#    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
#    if(WINDEPLOYQT_EXECUTABLE)
#        add_custom_command(TARGET metro POST_BUILD
#                COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-opengl-sw "$<TARGET_FILE:metro>"
#                COMMENT "Deploying Qt dependencies..."
#        )
#    endif()
#endif()

cmake_minimum_required(VERSION 3.20)
project(metro)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt's automatic tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON) # Automatically processes .qrc files
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(metro
        src/main.cpp
        src/MainWindow.cpp
        include/MainWindow.h
        src/MetroSystem.cpp
        include/MetroSystem.h
        src/MetroUkkonen.cpp
        include/MetroUkkonen.h
        src/resources.qrc
)

# Tell CMake where to look for your header files (.h)
target_include_directories(metro PRIVATE include)

target_link_libraries(metro PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

# Copy the data file from the data/ folder to the build folder so the app can find it
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/data/metro_data.txt"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

if(WIN32)
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET metro POST_BUILD
                COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-opengl-sw "$<TARGET_FILE:metro>"
                COMMENT "Deploying Qt dependencies..."
        )
    endif()
endif()