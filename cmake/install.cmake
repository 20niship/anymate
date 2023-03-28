include(CMakePackageConfigHelpers)
target_include_directories(movutl PUBLIC
    $<BUILD_INTERFACE:${ALL_INCLUDE_DIR}/src>
    $<INSTALL_INTERFACE:movutl>)

# バイナリ: 引数3個目にRUNTIMEを指定する。
# INSTALL(TARGETS lan_router RUNTIME DESTINATION bin)
# ライブラリ: 引数3個目にLIBRARY を指定する。
INSTALL(TARGETS movutl LIBRARY DESTINATION lib)
# それ以外(スクリプトやconfファイル): 引数1個目にFILESを指定する。
# INSTALL(FILES lan_interface.conf DESTINATION conf PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
install(TARGETS movutl EXPORT movutl-config)

install(EXPORT movutl-config
    NAMESPACE movutl::
    DESTINATION lib/cmake/movutl)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/  DESTINATION include)
