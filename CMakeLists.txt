# Copyright (C) 2017 Stefan Constantin
#
# This file is part of fanspeedcontrol.
#
# fanspeedcontrol is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# fanspeedcontrol is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with fanspeedcontrol. If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required (VERSION 3.3)
set(CMAKE_CXX_STANDARD 11)
project (fanspeedcontrol)
set(PACKAGE_AUTHOR "Stefan Constantin <stefan-constantin@gmx.de>")


set(CONFIG_FILE "" CACHE STRING "config file relative to CMAKE_INSTALL_PREFIX")
set(DEST "bin" CACHE STRING "destination directory relative to CMAKE_INSTALL_PREFIX")
set(CPACK_GENERATOR "not set" CACHE STRING "package generator of CPack")


include_directories(src)

set(SOURCE_FILES
src/fanspeedcontrol/config/ArgsAndConfigProcessor.cpp
src/fanspeedcontrol/config/ArgsAndConfigProcessor.h
src/fanspeedcontrol/devices/AbstractDevice.cpp
src/fanspeedcontrol/devices/AbstractDevice.h
src/fanspeedcontrol/devices/NvidiaGpu.cpp
src/fanspeedcontrol/devices/NvidiaGpu.h
src/fanspeedcontrol/main.cpp
src/fanspeedcontrol/observers/LoggerObserver.cpp
src/fanspeedcontrol/observers/LoggerObserver.h
src/fanspeedcontrol/observers/NotifyObserver.cpp
src/fanspeedcontrol/observers/NotifyObserver.h
src/fanspeedcontrol/observers/SoundObserver.cpp
src/fanspeedcontrol/observers/SoundObserver.h
src/patterns/observer/AbstractObserver.cpp
src/patterns/observer/AbstractObserver.h
src/patterns/observer/Observable.cpp
src/patterns/observer/Observable.h
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if(NOT CONFIG_FILE STREQUAL "")
	target_compile_definitions(${PROJECT_NAME} PUBLIC CONFIG_FILE="${CONFIG_FILE}")
endif()

set(LOCALE_DIR ${CMAKE_INSTALL_PREFIX}/share/locale)
target_compile_definitions(${PROJECT_NAME} PUBLIC CONFIG_DIR="${LOCALE_DIR}")

find_package(Boost REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
set(LIBS ${LIBS} ${X11_LIBRARIES})

find_package(Gettext REQUIRED)
GETTEXT_CREATE_TRANSLATIONS(src/fanspeedcontrol/locale/fanspeedcontrol.po ALL
src/fanspeedcontrol/locale/en.po
src/fanspeedcontrol/locale/de.po
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(LIBS ${LIBS} Threads::Threads)

find_package(PkgConfig REQUIRED)
pkg_search_module(LIBNOTIFY REQUIRED libnotify)
include_directories(${LIBNOTIFY_INCLUDE_DIRS})
set(LIBS ${LIBS} ${LIBNOTIFY_LIBRARIES})

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(NVCtrl)
include_directories(${NVCtrlLib_INCLUDE_DIR})
set(LIBS ${LIBS} ${NVCtrlLib_LIBRARY})

find_path(SPDLOG_INCLUDE_DIR spdlog.h /usr/include/spdlog)
include_directories(${SPDLOG_INCLUDE_DIR})

find_path(JSON_INCLUDE_DIR json.hpp)
include_directories(${JSON_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} ${LIBS})


INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${DEST})


set(CPACK_GENERATORS DEB)

if("${CPACK_GENERATOR}" STREQUAL "DEB")
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Stefan Constantin")
endif()

if(${CPACK_GENERATOR} IN_LIST CPACK_GENERATORS)
	include (CPack)
endif()

