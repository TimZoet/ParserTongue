include(parsertongueVersionString)

set(NAME parsertongue)
set(TYPE module)
set(INCLUDE_DIR "include/parsertongue")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/argument.h
    ${INCLUDE_DIR}/flag.h
    ${INCLUDE_DIR}/list.h
    ${INCLUDE_DIR}/parsable.h
    ${INCLUDE_DIR}/parser.h
    ${INCLUDE_DIR}/parser_tongue_exception.h
    ${INCLUDE_DIR}/parse_error.h
    ${INCLUDE_DIR}/value.h
)
 
set(SOURCES
    ${SRC_DIR}/argument.cpp
    ${SRC_DIR}/flag.cpp
    ${SRC_DIR}/parser.cpp
    ${SRC_DIR}/parser_tongue_exception.cpp
    ${SRC_DIR}/parse_error.cpp
)

make_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${PARSERTONGUE_VERSION}
    WARNINGS WERROR
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
)

install_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${PARSERTONGUE_VERSION}
    HEADERS "${HEADERS}"
)