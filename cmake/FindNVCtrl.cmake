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

# Variables:
# NVCtrl_FOUND - if NVCtrl is found
# NVCtrl_INCLUDE_DIRS - include directories of NVCtrl
# NVCtrl_LIBRARY - library of NVCtrl

find_library(NVCtrlLib_LIBRARY XNVCtrl /usr/lib)
find_path(NVCtrlLib_INCLUDE_DIR NVCtrlLib.h /usr/include/NVCtrl)

if(NVCtrl_LIBRARY AND NVCtrl_INCLUDE_DIR)
	set(NVCtrl_FOUND true)
else()
	set(NVCtrl_FOUND false)
endif()

