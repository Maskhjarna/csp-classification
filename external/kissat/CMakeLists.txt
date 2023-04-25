project(kissat C)

ExternalProject_Add(kissat
  GIT_REPOSITORY https://github.com/arminbiere/kissat
  GIT_TAG c25429fbce1b5c74d5372e39d681826b33ddaf18 # 3.0.0
  BUILD_IN_SOURCE true
  CONFIGURE_COMMAND
    <SOURCE_DIR>/configure --quiet
  BUILD_COMMAND
    make
  INSTALL_COMMAND
    mkdir -p <INSTALL_DIR>/lib &&
    cp <SOURCE_DIR>/build/libkissat.a <INSTALL_DIR>/lib &&
    cp -a <SOURCE_DIR>/src/. <INSTALL_DIR>/include/
  TEST_COMMAND
    make test
  UPDATE_COMMAND
    ""
  )
ExternalProject_Get_Property(kissat INSTALL_DIR)

file(MAKE_DIRECTORY "${INSTALL_DIR}/include")
file(MAKE_DIRECTORY "${INSTALL_DIR}/lib")

add_library(external_kissat STATIC IMPORTED GLOBAL)
target_include_directories(external_kissat INTERFACE "${INSTALL_DIR}/include")
set_target_properties(external_kissat PROPERTIES
  IMPORTED_LOCATION "${INSTALL_DIR}/lib/libkissat.a"
  INTERFACE_INCLUDE_DIRECTORIES "${INSTALL_DIR}/include")

add_dependencies(external_kissat kissat)
