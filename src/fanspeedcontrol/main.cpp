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

#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>
#include <variant>
#include <vector>

#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/program_options.hpp>
#include <libintl.h>

#include "config/ArgsAndConfigProcessor.h"
#include "devices/AbstractDevice.h"

const std::chrono::milliseconds MAX_SLEEP_TIME(100);

sig_atomic_t appStopFlag = 0;

void setAppStopFlag(int signal) {
	appStopFlag = 1;
}

int main(int argc, char *argv[]) {
	msc42::fanspeedcontrol::setLocale();

	std::variant<msc42::fanspeedcontrol::configuration, int> configurationOrErrorCode =
			msc42::fanspeedcontrol::processArguments(argc, argv);

	if (std::holds_alternative<int>(configurationOrErrorCode)) {
		return std::get<int>(configurationOrErrorCode);
	}

	const msc42::fanspeedcontrol::configuration configuration =
			std::move(std::get<msc42::fanspeedcontrol::configuration>(configurationOrErrorCode));

	std::signal(SIGTERM, setAppStopFlag);
	std::signal(SIGINT, setAppStopFlag);

	boost::interprocess::named_mutex mutex(boost::interprocess::open_or_create,
			msc42::fanspeedcontrol::DOMAIN_NAME.c_str());
	if (!mutex.try_lock()) {
		std::cout << gettext("Cannot start this fanspeedcontrol instance, "
				"because another instance has locked starting new instances.\n"
				"Terminate running instance to start a new instance.\n"
				"If you are sure, that no other instance is running, remove the lock with the option --remove-lock "
				"if you are not sure, but you want to start a new fanspeedcontrol instance, restart your system "
				"to terminate the possible running instance and remove the lock.\n"
				"Wrong usage of the option remove-lock can overheat your system.") << std::endl;
		return EXIT_FAILURE;
	}

	for (const std::unique_ptr<msc42::fanspeedcontrol::AbstractDevice> &device : configuration.devices) {
		device->notifyObservers(msc42::fanspeedcontrol::AbstractDevice::DEVICE_CONFIG, device->to_string(true));
	}

	try {
		while (appStopFlag == 0) {
			std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

			for (const std::unique_ptr<msc42::fanspeedcontrol::AbstractDevice> &device : configuration.devices) {
				device->setOptimalFanSpeed();
			}

			// sleep max MAX_SLEEP_TIME milliseconds to ensure to have enough time to call the destructor
			// between sigterm and sigkill by system shutdown
			bool notLastThreadSleepInThisIteration = true;
			while (notLastThreadSleepInThisIteration && appStopFlag == 0) {
				std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
				std::chrono::milliseconds processingTime =
						std::chrono::duration_cast<std::chrono::milliseconds>(now - begin);

				if (configuration.interval - processingTime >= MAX_SLEEP_TIME) {
					std::this_thread::sleep_for(MAX_SLEEP_TIME);
				} else {
					std::this_thread::sleep_for(configuration.interval - processingTime);
					notLastThreadSleepInThisIteration = false;
				}
			}
		}

		mutex.unlock();
	} catch (...) {
		// catch all exceptions because it is important to call destructor of a device if temperature is set once
	}

	return EXIT_SUCCESS;
}
