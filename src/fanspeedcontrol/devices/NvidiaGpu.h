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

#ifndef FANSPEEDCONTROL_DEVICES_NVIDIAGPU_H_
#define FANSPEEDCONTROL_DEVICES_NVIDIAGPU_H_

#include <string>

#include <X11/Xlib.h>

#include "AbstractDevice.h"

namespace msc42 {
namespace fanspeedcontrol {

class NvidiaGpu: public AbstractDevice {
public:
	NvidiaGpu(int id, int hysteresis, int warn, const std::map<int, int> &pairs, const std::string &displayName);
	virtual ~NvidiaGpu();

protected:
	const std::string displayName;
	Display *dpy;
	const unsigned int display_mask = 0u;

	virtual int getTemperature();
	virtual bool setFanSpeed(int speed);
	virtual bool setManualMode();
	virtual bool setAutomaticMode();
};

}
}

#endif /* FANSPEEDCONTROL_DEVICES_NVIDIAGPU_H_ */
