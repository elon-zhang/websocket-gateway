cmake_minimum_required(VERSION 2.8)
set(TARGET_NAME ${LIBPUBLIC})
project(${TARGET_NAME} CXX)

aux_source_directory(. LIST1)
aux_source_directory(./json LIST2)

if(WIN32)
    file(GLOB HEAD1 *.h)
	file(GLOB HEAD2 ./json/*.h)
	file(GLOB HEAD3 ./share/*.h)
	list(APPEND CODE ${LIST1} ${LIST2} ${HEAD1} ${HEAD2} ${HEAD3})
    add_library(${TARGET_NAME} SHARED ${CODE})
else(WIN32)
	SET_CCACHE_COMPLER()
	list(APPEND CODE ${LIST1} ${LIST2})
    add_library(${TARGET_NAME} SHARED ${CODE})
endif(WIN32)
