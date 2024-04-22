find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Charts)

add_library(LineGraphBase
    STATIC
        database.h database.cpp
)

target_include_directories(LineGraphBase PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(LineGraphBase
    PUBLIC
        Qt::Gui Qt::Widgets Qt::Sql Qt::Charts
)
