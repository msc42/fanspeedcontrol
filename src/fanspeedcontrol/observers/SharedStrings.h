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

#ifndef FANSPEEDCONTROL_OBSERVERS_SHAREDSTRINGS_H_
#define FANSPEEDCONTROL_OBSERVERS_SHAREDSTRINGS_H_

#include <string>

#include <libintl.h>

namespace msc42 {
namespace fanspeedcontrol {

	const std::string CONFIG_FILE_ERROR_MESSAGE = gettext("The configuration file is not valid.");
	const std::string READ_TEMPERATURE_ERROR_MESSAGE = gettext("Cannot read the temperature of at least one device.");
	const std::string MODE_AUTOMATIC_SET_MESSAGE = gettext("Set at least one device to automatic mode.");
	const std::string MODE_AUTOMATIC_ERROR_MESSAGE = gettext("Cannot set at least one device to automatic mode.");
	const std::string MODE_MANUAL_ERROR_MESSAGE = gettext("Cannot set of least one device to manual mode.");
	const std::string FAN_SET_ERROR_MESSAGE = gettext("Cannot set fan speed of at least one device.");
	const std::string TEMPERATURE_TOO_HIGH = gettext("Temperature of at least one device is very high.");

}
}

#endif /* FANSPEEDCONTROL_OBSERVERS_SHAREDSTRINGS_H_ */
