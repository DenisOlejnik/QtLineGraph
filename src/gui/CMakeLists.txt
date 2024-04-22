find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

qt_wrap_ui(UI_HEADERS
    mainwindow.ui
    linegraph.ui
    tabbar.ui
    tabtable.ui
    databaseviewer.ui
)

add_library(LineGraphGui
    STATIC
        mainwindow.h mainwindow.cpp
        linegraph.h linegraph.cpp
        tabbar.h tabbar.cpp
        tabtable.h tabtable.cpp
        databaseviewer.h databaseviewer.cpp
        ${UI_HEADERS}
)

target_include_directories(LineGraphGui PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(LineGraphGui
    PUBLIC
        Qt::Gui Qt::Widgets Qt::Sql
)
