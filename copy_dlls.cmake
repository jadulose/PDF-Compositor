# Copy Qt DLLs to target directory
if (WIN32) # only do this on windows
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}/../..")
    # platforms/qwindows.dll & styles/qwindowsvistastyle.dll
    if (NOT EXISTS "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    if (NOT EXISTS "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/styles/qwindowsvistastyle${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/styles/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/styles/qwindowsvistastyle${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/styles/qwindowsvistastyle${DEBUG_SUFFIX}.dll")
    endif ()
    foreach (QT_LIB ${REQUIRED_LIBS})
        if (NOT EXISTS "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Qt${QT_VERSION}${QT_LIB}${DEBUG_SUFFIX}.dll")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${QT_INSTALL_PATH}/bin/Qt${QT_VERSION}${QT_LIB}${DEBUG_SUFFIX}.dll"
                    "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
        endif ()
    endforeach (QT_LIB)
endif ()