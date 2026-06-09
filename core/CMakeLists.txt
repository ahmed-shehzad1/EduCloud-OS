cmake_minimum_required(VERSION 3.16)

project(EduCloudOSCore VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# 🌐 FIXED: Added Gui and Network components to the required dependencies
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

# 🛠️ FIXED: Synchronized missing header targets alongside implementation profiles
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
    taskmanagerapp.h
    taskmanagerapp.cpp
    filesystemapp.h
    filesystemapp.cpp   
    calculatorapp.h
    calculatorapp.cpp
    snakegameapp.h
    snakegameapp.cpp 
    browserapp.h  
    browserapp.cpp
    contextmenumanager.h
    contextmenumanager.cpp  
    logindialog.cpp
    logindialog.h
)

# 🌐 FIXED: Linked all necessary target runtime execution pipeline libraries
target_link_libraries(EduCloudOSCore PRIVATE 
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets 
    Qt6::Network
)