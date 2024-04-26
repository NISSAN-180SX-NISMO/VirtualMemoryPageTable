cmake_minimum_required(VERSION 3.27)
project(PageTable)

set(CMAKE_CXX_STANDARD 17)

add_executable(PageTable
        lab4.cpp
        lab4.h
        PageTable/PageTable.h
        PageFaultHandle/PageFaultHandlerInterface.h
        PageFaultHandle/FIFO.h
        PageFaultHandle/NFU.h
)