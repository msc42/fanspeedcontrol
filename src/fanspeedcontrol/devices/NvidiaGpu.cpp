// Copyright (C) 2017 Stefan Constantin
//
// This file is part of fanspeedcontrol.
//
// fanspeedcontrol is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// fanspeedcontrol is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fanspeedcontrol. If not, see <http://www.gnu.org/licenses/>.

#include "NvidiaGpu.h"

#include <iostream>
#include <sstream>
#include <string>

#include "NVCtrl/NVCtrl.h"
#include "NVCtrl/NVCtrlLib.h"
#include <X11/Xlib.h>

#include "AbstractDevice.h"

namespace msc42 {
namespace fanspeedcontrol {

NvidiaGpu::NvidiaGpu(int id, int hysteresis, int warn, const std::map<int, int> &pairs, const std::string &displayName)
: AbstractDevice("nvidia", id, hysteresis, warn, pairs), displayName(displayName) {
	dpy = XOpenDisplay(displayName.c_str());
}

NvidiaGpu::~NvidiaGpu() {
	if (manualModeWasSetAtLeastOnce) {
		if (setAutomaticMode()) {
			notifyObservers(DEVICE_TERMINATED, to_string());
		} else {
			notifyObservers(DEVICE_TERMINATED_ERROR, to_string());
		}
	}

	XCloseDisplay(dpy);
}

int NvidiaGpu::getTemperature() {
	int temperature;

	bool isTemperatureRead = XNVCTRLQueryTargetAttribute(dpy, NV_CTRL_TARGET_TYPE_GPU,
			id, display_mask, NV_CTRL_GPU_CORE_TEMPERATURE, &temperature);

	if (!isTemperatureRead) {
		return -274;
	}

	return temperature;
}

bool NvidiaGpu::setFanSpeed(int speed) {
	return XNVCTRLSetTargetAttributeAndGetStatus(dpy, NV_CTRL_TARGET_TYPE_COOLER,
			id, display_mask, NV_CTRL_THERMAL_COOLER_LEVEL, speed);
}

bool NvidiaGpu::setManualMode() {
	return XNVCTRLSetTargetAttributeAndGetStatus(dpy, NV_CTRL_TARGET_TYPE_GPU,
			id, display_mask, NV_CTRL_GPU_COOLER_MANUAL_CONTROL, NV_CTRL_GPU_COOLER_MANUAL_CONTROL_TRUE);
}

bool NvidiaGpu::setAutomaticMode() {
	return XNVCTRLSetTargetAttributeAndGetStatus(dpy, NV_CTRL_TARGET_TYPE_GPU,
			id, display_mask, NV_CTRL_GPU_COOLER_MANUAL_CONTROL, NV_CTRL_GPU_COOLER_MANUAL_CONTROL_FALSE);
}

}
}
