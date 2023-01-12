#    /fsm/include/CMakeLists.txt
#
#    @author Valery Mikhaylovsky
#    @date Jan 12, 2023

cmake_minimum_required(VERSION 3.12)

install(
    DIRECTORY fsm
    DESTINATION include
    FILES_MATCHING PATTERN *.h
)
