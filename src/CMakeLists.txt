add_executable(test test.cc test.h)
target_link_libraries(test PUBLIC ROOT::EG at_task_main)