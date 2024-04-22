add_executable(LineGraphApp)

target_sources(LineGraphApp
    PRIVATE
        main.cpp
)

target_link_libraries(LineGraphApp PRIVATE LineGraphGui LineGraphBase)


set_target_properties(LineGraphApp PROPERTIES WIN32_EXECUTABLE ON)

install(TARGETS LineGraphApp
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}    
    BUNDLE DESTINATION .
    COMPONENT runtime
)
