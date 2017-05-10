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

#ifndef FANSPEEDCONTROL_DEVICES_ABSTRACTDEVICE_H_
#define FANSPEEDCONTROL_DEVICES_ABSTRACTDEVICE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "patterns/observer/AbstractObserver.h"
#include "patterns/observer/Observable.h"

namespace msc42 {
namespace fanspeedcontrol {

class AbstractDevice : public msc42::patterns::Observable {
public:
	enum AbstractDeviceMessages : const int  {
		CONFIG_FILE_ERROR,
		TEMPERATUR_READ_ERROR,
		MODE_AUTOMATIC_SET,
		MODE_AUTOMATIC_SET_ERROR,
		MODE_MANUAL_SET_ERROR,
		FAN_SET,
		FAN_SET_ERROR,
		DEVICE_CONFIG,
		TEMPERATURE_WARN,
		DEVICE_TERMINATED,
		DEVICE_TERMINATED_ERROR
	};

	AbstractDevice(const std::string &type, int id, int hysteresis, int warn, const std::map<int, int> &pairs);
	virtual ~AbstractDevice();
	virtual void setOptimalFanSpeed();
	virtual std::string to_string(bool verbose = false) const;
	virtual bool checkIfValid() const;

protected:
	const std::string typeString;
	const int id;
	const int hysteresis;
	const int warn;
	const std::map<int, int> pairs;

	int currentFanSpeed = -1;
	bool automaticMode = false;
	bool manualModeWasSetAtLeastOnce = false;

	virtual int getTemperature() = 0;
	virtual bool setFanSpeed(int speed) = 0;
	virtual bool setManualMode() = 0;
	virtual bool setAutomaticMode() = 0;

	virtual int calculateOptimalFanSpeed(int currentTemperature) const;
	virtual int getFanSpeed(int currentTemperature, int hysteresis) const;
};

}
}

#endif /* FANSPEEDCONTROL_DEVICES_ABSTRACTDEVICE_H_ */
