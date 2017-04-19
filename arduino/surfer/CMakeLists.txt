
cmake_minimum_required (VERSION 2.8.11)
project (surfer)

add_definitions("-ggdb")
add_definitions("-pedantic")
add_definitions("-Wall")
add_definitions("-Wfatal-errors")
add_definitions("-DEMULATE_SERIAL")

set( surfer_c Channels.c SignalProcess.c StreamChannel.c x86/ConsoleIO.c)

add_executable (surfer_x86 
                ${surfer_c}
                x86/surfer_x86.c )

add_executable (surfer_test 
                ${surfer_c}
                x86/surfer_test.c )

target_link_libraries(surfer_test cunit m)

target_link_libraries(surfer_x86 m)

