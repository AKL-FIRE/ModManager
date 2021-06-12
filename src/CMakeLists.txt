add_executable(${PROJECT_NAME} main.cpp mainwindow.cpp mainwindow.h mainwindow.ui Recorder.cpp Recorder.h)
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})