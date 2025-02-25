project(opm-common C CXX)
cmake_minimum_required (VERSION 3.10)

option(SIBLING_SEARCH "Search for other modules in sibling directories?" ON)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)
set(OPM_MACROS_ROOT ${PROJECT_SOURCE_DIR})

option(ENABLE_ECL_INPUT "Enable eclipse input support?" ON)
option(ENABLE_ECL_OUTPUT "Enable eclipse output support?" ON)
option(ENABLE_MOCKSIM "Build the mock simulator for io testing" ON)
option(OPM_ENABLE_PYTHON "Enable python bindings?" OFF)
option(OPM_INSTALL_PYTHON "Enable python bindings?" OFF)
option(OPM_ENABLE_EMBEDDED_PYTHON "Enable embedded python?" OFF)

# Output implies input
if(ENABLE_ECL_OUTPUT)
  set(ENABLE_ECL_INPUT ON)
endif()

# And likewise, no input means no output
if(NOT ENABLE_ECL_INPUT)
  set(ENABLE_ECL_OUTPUT OFF)
endif()


# not the same location as most of the other projects; this hook overrides
macro (dir_hook)
endmacro (dir_hook)

# We need to define this variable in the installed cmake config file.
set(OPM_PROJECT_EXTRA_CODE_INSTALLED  "#ENABLE_ECL_INPUT is needed by opm-common-prereq.cmake
                                       set(ENABLE_ECL_INPUT ${ENABLE_ECL_INPUT})
                                       set(OPM_MACROS_ROOT ${CMAKE_INSTALL_PREFIX}/share/opm)
                                       list(APPEND CMAKE_MODULE_PATH \${OPM_MACROS_ROOT}/cmake/Modules)
                                       include(OpmPackage) #Make macros availabe after find_package(opm-common)")

set(OPM_PROJECT_EXTRA_CODE_INTREE "#ENABLE_ECL_INPUT is needed by opm-common-prereq.cmake
                                   set(ENABLE_ECL_INPUT ${ENABLE_ECL_INPUT})
                                   set(OPM_MACROS_ROOT ${OPM_MACROS_ROOT})
                                   list(APPEND CMAKE_MODULE_PATH \${OPM_MACROS_ROOT}/cmake/Modules)
                                   include(OpmPackage) #Make macros availabe after find_package(opm-common)")
if(ENABLE_ECL_OUTPUT)
  set(OPM_PROJECT_EXTRA_CODE_INSTALLED "${OPM_PROJECT_EXTRA_CODE_INSTALLED}
                                        set(COMPARE_ECL_COMMAND ${CMAKE_INSTALL_PREFIX}/bin${${name}_VER_DIR}/compareECL)
                                        set(OPM_PACK_COMMAND ${CMAKE_INSTALL_PREFIX}/bin${${name}_VER_DIR}/opmpack)
                                        set(RST_DECK_COMMAND ${CMAKE_INSTALL_PREFIX}/bin${${name}_VER_DIR}/rst_deck)")

  set(OPM_PROJECT_EXTRA_CODE_INTREE "${OPM_PROJECT_EXTRA_CODE_INTREE}
                                     set(COMPARE_ECL_COMMAND ${PROJECT_BINARY_DIR}/bin/compareECL)
                                     set(OPM_PACK_COMMAND ${PROJECT_BINARY_DIR}/bin/opmpack)
                                     set(RST_DECK_COMMAND ${PROJECT_BINARY_DIR}/bin/rst_deck)")
endif()

# project information is in dune.module. Read this file and set variables.
# we cannot generate dune.module since it is read by dunecontrol before
# the build starts, so it makes sense to keep the data there then.
include (OpmInit)
OpmSetPolicies()

# Look for the opm-tests repository; if found the variable
# HAVE_OPM_TESTS will be set to true.
include(Findopm-tests)

# list of prerequisites for this particular project; this is in a
# separate file (in cmake/Modules sub-directory) because it is shared
# with the find module
include (${project}-prereqs)

if(ENABLE_ECL_INPUT)
  # source_hook runs before config_hook and the former needs fmt, hence this
  # needs to be here.
  if(fmt_FOUND)
    # OpmSatellites will not add the library, do it here.
    list(APPEND opm-common_LIBRARIES fmt::fmt)
  else()
    add_definitions(-DFMT_HEADER_ONLY)
    include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/external/fmtlib/include)
    # Not sure why this is needed.
    list(APPEND EXTRA_INCLUDES ${PROJECT_SOURCE_DIR}/external/fmtlib/include)
  endif()
endif()

# read the list of components from this file (in the project directory);
# it should set various lists with the names of the files to include
include (CMakeLists_files.cmake)

macro (config_hook)
  if(ENABLE_ECL_INPUT)
    if(NOT cjson_FOUND)
      list(APPEND EXTRA_INCLUDES ${PROJECT_SOURCE_DIR}/external/cjson)
    endif()

    # For this project
    include_directories(${EXTRA_INCLUDES} ${PROJECT_BINARY_DIR}/include)
    # For downstreams
    list(APPEND EXTRA_INCLUDES ${PROJECT_BINARY_DIR}/include)
    set(OPM_PROJECT_EXTRA_CODE_INTREE "${OPM_PROJECT_EXTRA_CODE_INTREE}
                                       list(APPEND opm-common_INCLUDE_DIRS ${EXTRA_INCLUDES})")
    if(ENABLE_ECL_INPUT)
      set(OPM_PROJECT_EXTRA_CODE_INTREE "${OPM_PROJECT_EXTRA_CODE_INTREE}
                                         set(HAVE_ECL_INPUT 1)")
      set(OPM_PROJECT_EXTRA_CODE_INSTALLED "${OPM_PROJECT_EXTRA_CODE_INSTALLED}
                                            set(HAVE_ECL_INPUT 1)")
    endif()
    if(ENABLE_ECL_OUTPUT)
      set(OPM_PROJECT_EXTRA_CODE_INTREE "${OPM_PROJECT_EXTRA_CODE_INTREE}
                                         set(HAVE_ECL_OUTPUT 1)")
      set(OPM_PROJECT_EXTRA_CODE_INSTALLED "${OPM_PROJECT_EXTRA_CODE_INSTALLED}
                                            set(HAVE_ECL_OUTPUT 1)")
    endif()

    find_package(Boost COMPONENTS filesystem regex system unit_test_framework)

    if (HAVE_DYNAMIC_BOOST_TEST)
      set_target_properties(Boost::unit_test_framework PROPERTIES INTERFACE_COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK=1)
    endif()
  endif()
endmacro (config_hook)

macro (prereqs_hook)
endmacro (prereqs_hook)

macro (sources_hook)
  if(ENABLE_ECL_INPUT)
    # Keyword generation
    include(GenerateKeywords.cmake)

    # Append generated sources
    list(INSERT opm-common_SOURCES 0 ${PROJECT_BINARY_DIR}/ParserInit.cpp)
    foreach (name A B C D E F G H I J K L M N O P Q R S T U V W X Y Z)
        list(INSERT opm-common_SOURCES 0 ${PROJECT_BINARY_DIR}/ParserKeywords/${name}.cpp)
    endforeach()
    if (OPM_ENABLE_PYTHON)
      list(INSERT opm-common_SOURCES 0 ${PROJECT_BINARY_DIR}/python/cxx/builtin_pybind11.cpp)
    endif()
  endif()
  set_source_files_properties(src/opm/parser/eclipse/Python/Python.cpp
                              PROPERTIES COMPILE_FLAGS -Wno-shadow)
endmacro (sources_hook)

macro (fortran_hook)
endmacro (fortran_hook)

macro (files_hook)
endmacro (files_hook)

macro (tests_hook)
  if(ENABLE_ECL_INPUT)
    include(ExtraTests.cmake)
  endif()
endmacro (tests_hook)

macro (install_hook)
  install(DIRECTORY ${PROJECT_BINARY_DIR}/include/
          DESTINATION include
          PATTERN *.hpp)
endmacro (install_hook)

# If opm-common is configured to embed the python interpreter we must make sure
# that all downstream modules link libpython transitively. Due to the required
# integration with Python+cmake machinery provided by pybind11 this is done by
# manually adding to the opm-common_LIBRARIES variable here, and not in the
# OpmnLibMain function. Here only the library dependency is implemented, the
# bulk of the python configuration is further down in the file.
if (OPM_ENABLE_PYTHON)
  # We need to be compatible with older CMake versions
  # that do not offer FindPython3
  # e.g. Ubuntu LTS 18.04 uses cmake 3.10
  if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
    find_package(PythonInterp REQUIRED)
    if (OPM_ENABLE_EMBEDDED_PYTHON)
      find_package(PythonLibs REQUIRED)
      list(APPEND opm-common_LIBRARIES ${PYTHON_LIBRARIES})
    endif()
    if(PYTHON_VERSION_MAJOR LESS 3)
      message(SEND_ERROR "OPM requires version 3 of Python but only version ${PYTHON_VERSION_STRING} was found")
    endif()
    set(Python3_EXECUTABLE ${PYTHON_EXECUTABLE})
    set(Python3_LIBRARIES ${PYTHON_LIBRARIES})
  else()
    # Be backwards compatible.
    if(PYTHON_EXECUTABLE AND NOT Python3_EXECUTABLE)
      set(Python3_EXECUTABLE ${PYTHON_EXECUTABLE})
    endif()
    if (OPM_ENABLE_EMBEDDED_PYTHON)
      find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
      get_target_property(_lib_path Python3::Python IMPORTED_LOCATION)
      set(PYTHON_LIBRARY ${_lib_path})
      set(PYTHON_LIBRARIES {PYTHON_LIBRARY})
      list(APPEND opm-common_LIBRARIES ${PYTHON_LIBRARY})
      set(PYTHON_INCLUDE_DIRS ${Python3_INCLUDE_DIRS})
    else()
      find_package(Python3 REQUIRED COMPONENTS Interpreter)
    endif()
    # Compatibility settings for PythonInterp and PythonLibs
    # used e.g. in FindCwrap, pybind11
    set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
  endif()
  # We always need the PYTHON_INCLUDE_DIR. Unfortunately
  # When we build pypi packages CMake will fail to determine
  # these via the usual find_package(PythonLibs or
  # find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
  # Hence we overwrite them here.
  if(NOT PYTHON_INCLUDE_DIRS)
    execute_process(COMMAND "${PYTHON_EXECUTABLE}" "-c" "from distutils import sysconfig; print(sysconfig.get_python_inc(plat_specific=True), end=\"\");"
      RESULT_VARIABLE _PYTHON_DIR_SUCCESS
      OUTPUT_VARIABLE PYTHON_INCLUDE_DIR
      ERROR_VARIABLE _PYTHON_ERROR_VALUE)
    if(NOT _PYTHON_DIR_SUCCESS MATCHES 0)
      message(FATAL_ERROR "Could not determine Python include directory. Error: ${_PYTHON_ERROR_VALUE}.")
    endif()
    set(PYTHON_INCLUDE_DIRS ${PYTHON_INCLUDE_DIR})
  endif()
  find_package(pybind11 2.2 CONFIG)
  if (NOT pybind11_FOUND)
    # Use full path for reuse with pypi
    set(pybind11_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/python/pybind11/include ${PYTHON_INCLUDE_DIRS})
  endif()
endif()


# all setup common to the OPM library modules is done here
include (OpmLibMain)

if (ENABLE_MOCKSIM)
  add_library(mocksim
              msim/src/msim.cpp)
  target_link_libraries(mocksim opmcommon)
  target_include_directories(mocksim PUBLIC msim/include)
  add_executable(msim examples/msim.cpp)
  target_link_libraries(msim mocksim)

  if (Boost_UNIT_TEST_FRAMEWORK_FOUND)
    set(_libs mocksim opmcommon
      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

    foreach( test test_msim test_msim_ACTIONX test_msim_EXIT)
      opm_add_test(${test} SOURCES tests/msim/${test}.cpp
        LIBRARIES ${_libs}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/tests
        CONDITION ${HAVE_ECL_INPUT})
    endforeach()
  endif()
endif()

if (ENABLE_SCHEDULE_DEBUG)
  add_compile_definitions( SCHEDULE_DEBUG )
endif()

# Build the compare utilities
if(ENABLE_ECL_INPUT)
  add_executable(compareECL
    test_util/EclFilesComparator.cpp
    test_util/EclRegressionTest.cpp
    test_util/compareECL.cpp
    )

  add_executable(convertECL
    test_util/convertECL.cpp
    )

  add_executable(summary
    test_util/summary.cpp
    )

  add_executable(rewriteEclFile
    test_util/rewriteEclFile.cpp
    )

  foreach(target compareECL convertECL summary rewriteEclFile)
    target_link_libraries(${target} opmcommon)
    install(TARGETS ${target} DESTINATION bin)
  endforeach()

  # Add the tests
  set(_libs opmcommon
            ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

  opm_add_test(test_EclFilesComparator
    CONDITION
      ENABLE_ECL_INPUT AND Boost_UNIT_TEST_FRAMEWORK_FOUND
    SOURCES
      tests/test_EclFilesComparator.cpp
      test_util/EclFilesComparator.cpp
    LIBRARIES
      ${_libs}
    WORKING_DIRECTORY
      ${PROJECT_BINARY_DIR}/tests
    )

  opm_add_test(test_EclRegressionTest
    CONDITION
      ENABLE_ECL_INPUT AND Boost_UNIT_TEST_FRAMEWORK_FOUND
    SOURCES
      tests/test_EclRegressionTest.cpp
      test_util/EclFilesComparator.cpp
      test_util/EclRegressionTest.cpp
    LIBRARIES
      ${_libs}
    WORKING_DIRECTORY
      ${PROJECT_BINARY_DIR}/tests
    )

  foreach(test test_EclIO test_EGrid test_ERft test_ERst test_ESmry test_EInit test_ExtESmry)
    opm_add_test(${test} CONDITION ENABLE_ECL_INPUT AND Boost_UNIT_TEST_FRAMEWORK_FOUND
                         LIBRARIES ${_libs}
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
  endforeach()
endif()

# Install build system files and documentation
install(DIRECTORY cmake
  DESTINATION share/opm USE_SOURCE_PERMISSIONS
  PATTERN "OPM-CMake.md" EXCLUDE)

install(FILES cmake/OPM-CMake.md
  DESTINATION ${CMAKE_INSTALL_DOCDIR})

# Install tab completion skeleton
install(FILES etc/opm_bash_completion.sh.in DESTINATION share/opm/etc)

if (OPM_ENABLE_PYTHON)
  # -------------------------------------------------------------------------
  # 1: Wrap C++ functionality in Python
  if (EXISTS "/etc/debian_version")
    set(PYTHON_PACKAGE_PATH "dist-packages")
  else()
    set(PYTHON_PACKAGE_PATH "site-packages")
  endif()
  if(PYTHON_VERSION_MAJOR)
    set(PY_MAJOR ${PYTHON_VERSION_MAJOR})
  else()
    set(PY_MAJOR ${Python3_VERSION_MAJOR})
  endif()
  if(PYTHON_VERSION_MINOR)
    set(PY_MINOR ${PYTHON_VERSION_MINOR})
  else()
    set(PY_MINOR ${Python3_VERSION_MINOR})
  endif()
  set(PYTHON_INSTALL_PREFIX "lib/python${PY_MAJOR}.${PY_MINOR}/${PYTHON_PACKAGE_PATH}" CACHE STRING "Subdirectory to install Python modules in")

  make_directory(${PROJECT_BINARY_DIR}/python)
  get_target_property(_opmcommon_include_dirs opmcommon INCLUDE_DIRECTORIES)
  list(APPEND _opmcommon_include_dirs ${_ecl_include_dirs})
  string(REPLACE ";" ":" _setup_include_dirs "${_opmcommon_include_dirs}")

  get_target_property(_opmcommon_lib_dirs opmcommon LINK_DIRECTORIES)
  if (CMAKE_PREFIX_PATH)
    list(APPEND _opmcommon_lib_dirs ${PROJECT_BINARY_DIR}/lib ${CMAKE_PREFIX_PATH}/${CMAKE_INSTALL_LIBDIR})
  else()
    list(APPEND _opmcommon_lib_dirs ${PROJECT_BINARY_DIR}/lib)
  endif()
  string(REPLACE ";" ":" _setup_lib_dirs "${_opmcommon_lib_dirs}")

  if (USE_RUNPATH)
    set (_python_rpath_list)
    if (CMAKE_PREFIX_PATH)
      foreach(path ${CMAKE_PREFIX_PATH})
        list(APPEND _python_rpath_list "${path}/${CMAKE_INSTALL_LIBDIR}")
      endforeach()
    endif()

    if (BUILD_SHARED_LIBS)
      list(APPEND _python_rpath_list "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    endif()

    if (_python_rpath_list)
      string(REPLACE ";" ":" _rpath "${_python_rpath_list}")
      set( _rpath_arg "--rpath=${_rpath}")
    else()
      set(_rpath_arg "")
    endif()
  else()
    set( _rpath_arg "")
  endif()

  set(opm-common_PYTHON_PACKAGE_VERSION ${OPM_PYTHON_PACKAGE_VERSION_TAG})

  # Generate versioned setup.py
  if (pybind11_INCLUDE_DIRS)
    string(REGEX REPLACE ";" "', '"  _tmp "${pybind11_INCLUDE_DIRS}")
    set(SETUP_PY_PYBIND_INCLUDE_DIR "'${_tmp}'")
  endif()

  if (TARGET fmt::fmt)
     get_target_property(SETUP_PY_FMT_LIBS fmt::fmt LOCATION)
     get_target_property(FMT_FLAGS fmt::fmt INTERFACE_COMPILE_DEFINITIONS)
     foreach(flag ${FMT_FLAGS})
       set(SETUP_PY_FMT_FLAGS "${SETUP_PY_FMT_FLAGS} '-D${flag}',")
     endforeach()
     list(APPEND opm-common_PYTHON_LINKAGE "'${SETUP_PY_FMT_LIBS}'")
  else()
    set(SETUP_PY_FMT_FLAGS "'-DFMT_HEADER_ONLY'")
  endif()

  configure_file (${PROJECT_SOURCE_DIR}/python/setup.py.in ${PROJECT_BINARY_DIR}/python/setup.py)
  file(COPY ${PROJECT_SOURCE_DIR}/python/README.md DESTINATION ${PROJECT_BINARY_DIR}/python)
  execute_process(COMMAND ${Python3_EXECUTABLE} target_name.py
                  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/python
                  OUTPUT_VARIABLE python_lib_target)

  add_custom_target(copy_python ALL
    COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/python/install.py ${PROJECT_SOURCE_DIR}/python ${PROJECT_BINARY_DIR} 0)

  add_custom_command(OUTPUT python/opm/${python_lib_target}
                     DEPENDS ${PYTHON_CXX_DEPENDS}
                     DEPENDS copy_python
                     COMMAND ${Python3_EXECUTABLE} ${PROJECT_BINARY_DIR}/python/setup.py
                                                      build
                                                      build_ext
                                                      --build-lib=${PROJECT_BINARY_DIR}/python
                                                      --library-dirs=${_setup_lib_dirs}
                                                      ${_rpath_arg}
                                                      --include-dirs=${_setup_include_dirs}
                     WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/python
                     COMMENT "Building python bindings at python/opm/${python_lib_target}")
  add_custom_target(opmcommon_python ALL DEPENDS python/opm/${python_lib_target})
  add_dependencies(opmcommon_python opmcommon)

  # The install target is based on manually copying the python file tree to the
  # installation area with a small installation script 'install.py'. Would have
  # preferred to use standard setup.py install, but the setup.py based solution
  # refuses to install to a location which the current python executable can not
  # load from, and the use of eggs in the setup.py based installation makes
  # debugging quite difficult.
  #
  # Since the installation of Python code is nonstandard it is protected by an
  # extra cmake switch, OPM_INSTALL_PYTHON. If you prefer you can still invoke
  # setup.py install manually - optionally with the generated script
  # setup-install.sh - and completely bypass cmake in the installation phase.
  if (OPM_INSTALL_PYTHON)
    install( CODE "execute_process(COMMAND ${Python3_EXECUTABLE} ${PROJECT_BINARY_DIR}/python/install.py ${PROJECT_BINARY_DIR}/python/opm ${DEST_PREFIX}${CMAKE_INSTALL_PREFIX}/${PYTHON_INSTALL_PREFIX} 1)")
  endif()

  # Observe that if the opmcommon library has been built as a shared library the
  # python library opmcommon_python will in general not find it runtime while
  # testing.
  add_test(NAME python_tests
           WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/python
           COMMAND ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${PROJECT_BINARY_DIR}/lib ${Python3_EXECUTABLE} setup.py build_ext --dry-run --build-lib ${PROJECT_BINARY_DIR}/python test
           )

  set_target_properties(opmcommon PROPERTIES POSITION_INDEPENDENT_CODE ON)
  set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_BINARY_DIR}/python)

  # -------------------------------------------------------------------------
  # Let cmake configure some small shell scripts which can be used to simplify
  # building, testing and installation of the Python extensions.
  configure_file(python/setup-build.sh.in tmp/setup-build.sh)
  file( COPY ${PROJECT_BINARY_DIR}/tmp/setup-build.sh
        DESTINATION ${PROJECT_BINARY_DIR}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE )

  configure_file(python/setup-package.sh.in tmp/setup-package.sh)
  file( COPY ${PROJECT_BINARY_DIR}/tmp/setup-package.sh
        DESTINATION ${PROJECT_BINARY_DIR}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE )

  configure_file(python/setup-test.sh.in tmp/setup-test.sh)
  file( COPY ${PROJECT_BINARY_DIR}/tmp/setup-test.sh
        DESTINATION ${PROJECT_BINARY_DIR}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE )

  configure_file(python/setup-install.sh.in tmp/setup-install.sh)
  file( COPY ${PROJECT_BINARY_DIR}/tmp/setup-install.sh
        DESTINATION ${PROJECT_BINARY_DIR}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE )

  configure_file(python/enable-python.sh.in enable-python.sh)

  # -------------------------------------------------------------------------
  # 2: Embed the Python interpreter for keywords like PYACTION and PYINPUT
  target_include_directories(opmcommon SYSTEM PRIVATE "${pybind11_INCLUDE_DIRS}")
  if (OPM_ENABLE_EMBEDDED_PYTHON)
    target_link_libraries(opmcommon PUBLIC ${PYTHON_LIBRARY})
    add_definitions(-DEMBEDDED_PYTHON)
  endif()
else()
  if (OPM_ENABLE_EMBEDDED_PYTHON)
    message(WARNING "Inconsistent settings OPM_ENABLE_PYTHON / OPM_ENABLE_EMBEDDED_PYTHON")
    set(OPM_ENABLE_EMBEDDED_PYTHON OFF CACHE BOOL "Enable embedded python?" FORCE)
  endif()
endif()

install(DIRECTORY docs/man1 DESTINATION ${CMAKE_INSTALL_MANDIR}
  FILES_MATCHING PATTERN "*.1")
