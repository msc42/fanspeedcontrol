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

#ifndef FANSPEEDCONTROL_CONFIG_ARGSANDCONFIGPROCESSOR_H_
#define FANSPEEDCONTROL_CONFIG_ARGSANDCONFIGPROCESSOR_H_

#include <chrono>
#include <string>
#include <vector>

#include "fanspeedcontrol/devices/AbstractDevice.h"

namespace msc42 {
namespace fanspeedcontrol {

const std::string APP_NAME = "fanspeedcontrol";
const std::string DOMAIN_NAME = "msc42_" + APP_NAME;

struct configuration {
	std::vector<std::unique_ptr<AbstractDevice>> devices;
	std::chrono::milliseconds interval;
};

void setLocale();
configuration processArguments(int argc, char *argv[], int &errorCode);

}
}

#endif /* FANSPEEDCONTROL_CONFIG_ARGSANDCONFIGPROCESSOR_H_ */
