add_library(Browser STATIC
    searchtoolbar.cpp
    webbridge.cpp
    webcontrol.cpp
    webview.cpp
)

# find_package(Qt${QT_VERSION_MAJOR} COMPONENTS WebChannel WebEngineWidgets REQUIRED)
# target_link_libraries(Browser PRIVATE Qt${QT_VERSION_MAJOR}::WebChannel Qt${QT_VERSION_MAJOR}::WebEngineWidgets)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network Widgets REQUIRED)
target_link_libraries(Browser PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network qlitehtml)
