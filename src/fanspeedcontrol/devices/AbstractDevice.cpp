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

#include "AbstractDevice.h"

#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <libintl.h>

namespace msc42 {
namespace fanspeedcontrol {

const int MIN_TEMPERATURE_VALID = 0;
const int MAX_TEMPERATURE_VALID = 120;
const int MAX_HYSTERESIS_VALID = 60;

AbstractDevice::AbstractDevice(const std::string &typeString, int id,
		int hysteresis, int warn, const std::map<int, int> &pairs)
: typeString(typeString), id(id), hysteresis(hysteresis), warn(warn), pairs(pairs) {
}

AbstractDevice::~AbstractDevice() {
}

void AbstractDevice::setOptimalFanSpeed() {
	int temperature = getTemperature();
	if (temperature < MIN_TEMPERATURE_VALID || temperature > MAX_TEMPERATURE_VALID) {
		notifyObservers(TEMPERATUR_READ_ERROR, to_string());
		if (automaticMode || setAutomaticMode()) {
			currentFanSpeed = -1;
			automaticMode = true;
			notifyObservers(MODE_AUTOMATIC_SET, to_string());
			return;
		} else {
			notifyObservers(MODE_AUTOMATIC_SET_ERROR, to_string());
			return;
		}
	}

	if (temperature >= warn) {
		notifyObservers(TEMPERATURE_WARN, to_string(), std::to_string(temperature));
	}

	int optimalFanSpeed = calculateOptimalFanSpeed(temperature);
	if (currentFanSpeed != optimalFanSpeed) {
		if (setManualMode()) {
			automaticMode = false;
			manualModeWasSetAtLeastOnce = true;
		} else {
			notifyObservers(MODE_MANUAL_SET_ERROR, to_string());
		}

		if (setFanSpeed(optimalFanSpeed)) {
			currentFanSpeed = optimalFanSpeed;
			manualModeWasSetAtLeastOnce = true;
			notifyObservers(FAN_SET, to_string(), std::to_string(currentFanSpeed));
		} else {
			notifyObservers(FAN_SET_ERROR, to_string());
			if (automaticMode || setAutomaticMode()) {
				currentFanSpeed = -1;
				automaticMode = true;
				notifyObservers(MODE_AUTOMATIC_SET, to_string());
			} else {
				notifyObservers(MODE_AUTOMATIC_SET_ERROR, to_string());
			}
		}
	}
}

std::string AbstractDevice::to_string(bool verbose) const {
	std::stringstream s;
	s << "{\"type\":\"" << typeString << "\", \"id\":" << id;

	if (verbose) {
		s << ", \"hysteresis\":" << hysteresis << ", \"warn\":" << warn << ", ";

		bool notFirstElement = false;
		for (std::pair<const int, int> pair : pairs) {
			if (notFirstElement) {
				s << ", ";
			} else {
				notFirstElement = true;
			}

			s << "\"" << pair.first << "\":" << pair.second;
		}
	}

	s << "}";

	return s.str();
}

int AbstractDevice::getFanSpeed(int currentTemperature, int hysteresis) const {
	for (const std::pair<const int, int>& kv : pairs) {
		if (currentTemperature < kv.first - hysteresis) {
			return kv.second;
		}
	}

	return 100;
}

int AbstractDevice::calculateOptimalFanSpeed(int currentTemperature) const {
	int optimalFanSpeedWithoutHysteresis = getFanSpeed(currentTemperature, 0);

	if (optimalFanSpeedWithoutHysteresis < currentFanSpeed) {
		return getFanSpeed(currentTemperature, hysteresis);
	}

	return optimalFanSpeedWithoutHysteresis;
}

bool AbstractDevice::checkIfValid() const {
	if (hysteresis < 0 || hysteresis > MAX_HYSTERESIS_VALID) {
		return false;
	}

	if (warn < MIN_TEMPERATURE_VALID || warn > MAX_TEMPERATURE_VALID) {
		return false;
	}

	int oldFanSpeed = -1;

	for (std::pair<const int, int> pair : pairs) {
		if (pair.first < MIN_TEMPERATURE_VALID || pair.first > MAX_TEMPERATURE_VALID) {
			return false;
		}

		if (pair.second < oldFanSpeed) {
			return false;
		}

		oldFanSpeed = pair.second;
	}
	return true;
}

}
}
