set(APP_SOURCES "${APP_NAME}.cc")

add_executable(${APP_NAME} ${APP_SOURCES})
target_link_libraries(${APP_NAME} ${APP_LINK_LIBRARIES})
install(TARGETS ${APP_NAME} DESTINATION "${CMAKE_SOURCE_DIR}/install") 
