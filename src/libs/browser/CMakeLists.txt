add_library(Browser STATIC
    searchtoolbar.cpp
    webbridge.cpp
    webcontrol.cpp
    webview.cpp
)

target_link_libraries(Browser)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network Widgets REQUIRED)
target_link_libraries(Browser Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network qlitehtml)
