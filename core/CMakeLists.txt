cmake_minimum_required(VERSION 3.16)

project(EduCloudOSCore VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# 🔴 FIX: Added "Widgets" to the required components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 🔴 FIX: Added mainwindow.h and mainwindow.cpp to the build
add_executable(EduCloudOSCore
    main.cpp
    desktop.h
    desktop.cpp
    mainwindow.h      
    mainwindow.cpp    
    job.h
    scheduler.h
    scheduler.cpp
    systemmonitor.h
    systemmonitor.cpp
    processtable.h      
    processtable.cpp    
    memorymanager.h     
    memorymanager.cpp   
    bankers.h           
    bankers.cpp  
    taskmanagerapp.cpp
    filesystemapp.cpp   
    calculatorapp.h
    calculatorapp.cpp
    snakegameapp.h
    snakegameapp.cpp 
    browserapp.cpp
    browserapp.h  
    contextmenumanager.h
    contextmenumanager.cpp  
)
# 🔴 FIX: Linked the Widgets library
target_link_libraries(EduCloudOSCore PRIVATE Qt6::Core Qt6::Widgets)