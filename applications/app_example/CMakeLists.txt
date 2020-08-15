set(NAME app_example)
set(TYPE application)
set(INCLUDE_DIR "include/app_example")
set(SRC_DIR "src")

set(HEADERS
	
)
 
set(SOURCES
	${SRC_DIR}/main.cpp
)

set(DEPS_PUBLIC
	parsertongue
)

make_target(TYPE ${TYPE} NAME ${NAME} STARTUP HEADERS "${HEADERS}" SOURCES "${SOURCES}" DEPS_PUBLIC "${DEPS_PUBLIC}")