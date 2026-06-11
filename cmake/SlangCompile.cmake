function (add_slang_shader_target TARGET)
    cmake_parse_arguments ("SHADER" "" "" "SOURCES" ${ARGN})
    set (SHADERS_DIR ${CMAKE_SOURCE_DIR}/resources/shaders/slang)

    message(STATUS "Slang Shader dir: ${SHADERS_DIR}")
    set (ENTRY_POINTS
            -entry vertMain -stage vertex
            -entry fragMain -stage fragment
    )
    add_custom_command (
            OUTPUT ${SHADERS_DIR}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}
    )
    add_custom_command (
            OUTPUT  ${SHADERS_DIR}/slang.spv
            COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SOURCES} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_POINTS} -o slang.spv
            WORKING_DIRECTORY ${SHADERS_DIR}
            DEPENDS ${SHADERS_DIR} ${SHADER_SOURCES}
            COMMENT "Compiling Slang Shaders"
            VERBATIM
    )
    add_custom_target (${TARGET} DEPENDS ${SHADERS_DIR}/slang.spv)
endfunction()

set(SHADER_SLANG_SOURCES
        ${CMAKE_SOURCE_DIR}/resources/shaders/slang/vktriangle.slang
)
add_slang_shader_target( slangShaders SOURCES ${SHADER_SLANG_SOURCES})
add_dependencies(${PROJECT_NAME} slangShaders)