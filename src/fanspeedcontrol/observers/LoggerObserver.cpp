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

#include "LoggerObserver.h"

#include <chrono>
#include <iostream>
#include <memory>

#include <boost/format.hpp>
#include <libintl.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/syslog_sink.h>

#include "fanspeedcontrol/devices/AbstractDevice.h"
#include "patterns/observer/Observable.h"
#include "SharedStrings.h"

namespace msc42 {
namespace fanspeedcontrol {

LoggerObserver::LoggerObserver(const std::chrono::milliseconds &timeToLogRepeatedError, const std::string &logLevel,
		const std::string &appName, const std::string &logFile)
:timeToLogRepeatedError(timeToLogRepeatedError) {
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	lastLoggedTemperatureError = now - timeToLogRepeatedError - std::chrono::milliseconds(1);
	lastLoggedModeAutomaticSet = now - timeToLogRepeatedError -	std::chrono::milliseconds(1);
	lastLoggedModeAutomaticSetError = now - timeToLogRepeatedError - std::chrono::milliseconds(1);
	lastLoggedModeManualSetError = now - timeToLogRepeatedError - std::chrono::milliseconds(1);
	lastLoggedFanSetError = now - timeToLogRepeatedError - std::chrono::milliseconds(1);
	lastLoggedTemperatureWarn = now - timeToLogRepeatedError - std::chrono::milliseconds(1);

	try {
		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());

		if (!logFile.empty()) {
			try {
				sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFile, 50000, 1));
			} catch (const spdlog::spdlog_ex &e) {
				std::cout << gettext("Cannot create file logger.") << std::endl;
			}
		}

#ifdef SPDLOG_ENABLE_SYSLOG
		sinks.push_back(std::make_shared<spdlog::sinks::syslog_sink>(appName));
#endif

		logger = std::make_shared<spdlog::logger>(appName, begin(sinks), end(sinks));
		spdlog::register_logger(logger);
		spdlog::set_pattern(std::string(gettext("%Y-%m-%d %H:%M:%S")) + " [%l] %v");

		if (logLevel == "debug") {
			spdlog::set_level(spdlog::level::debug);
		} else if (logLevel == "info") {
			spdlog::set_level(spdlog::level::info);
		} else if (logLevel == "error") {
			spdlog::set_level(spdlog::level::err);
		} else {
			spdlog::set_level(spdlog::level::info);
		}

	} catch (const spdlog::spdlog_ex &e) {
		std::cout << gettext("Cannot create logger.") << std::endl;
	}
}

LoggerObserver::~LoggerObserver() {
}

bool LoggerObserver::notify(int messageId, const std::string &message1, const std::string &message2) {
	if (!logger) {
		return false;
	}

	std::chrono::steady_clock::time_point now;

	switch (messageId) {

	case AbstractDevice::CONFIG_FILE_ERROR:
		logger->error(CONFIG_FILE_ERROR_MESSAGE);
		logger->flush();
		break;

	case AbstractDevice::TEMPERATUR_READ_ERROR:
		now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLoggedTemperatureError)
				>= timeToLogRepeatedError) {
			lastLoggedTemperatureError = now;
			logger->error(READ_TEMPERATURE_ERROR_MESSAGE);
			logger->flush();
		}
		break;

	case AbstractDevice::MODE_AUTOMATIC_SET:
		now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLoggedModeAutomaticSet)
				>= timeToLogRepeatedError) {
			lastLoggedModeAutomaticSet = now;
			logger->error(MODE_AUTOMATIC_SET_MESSAGE);
			logger->flush();
		}
		break;

	case AbstractDevice::MODE_AUTOMATIC_SET_ERROR:
		now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLoggedModeAutomaticSetError)
				>= timeToLogRepeatedError) {
			lastLoggedModeAutomaticSetError = now;
			logger->error(MODE_AUTOMATIC_ERROR_MESSAGE);
			logger->flush();
		}
		break;

	case AbstractDevice::MODE_MANUAL_SET_ERROR:
		now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLoggedModeManualSetError)
				>= timeToLogRepeatedError) {
			lastLoggedModeManualSetError = now;
			logger->error(MODE_MANUAL_ERROR_MESSAGE);
			logger->flush();
		}
		break;

	case AbstractDevice::FAN_SET:
		logger->debug((boost::format(gettext("Fan of %s is set to %s.")) % message1 % message2).str());
		logger->flush();
		break;

	case AbstractDevice::FAN_SET_ERROR:
		now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLoggedFanSetError)
				>= timeToLogRepeatedError) {
			lastLoggedFanSetError = now;
			logger->error(FAN_SET_ERROR_MESSAGE);
			logger->flush();
		}
		break;

	case AbstractDevice::DEVICE_CONFIG:
		logger->info((boost::format(gettext("Valid configuration of %s")) % message1).str());
		logger->flush();
		break;

	case AbstractDevice::TEMPERATURE_WARN:
		now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLoggedTemperatureWarn)
				>= timeToLogRepeatedError) {
			lastLoggedTemperatureWarn = now;
			logger->error(TEMPERATURE_TOO_HIGH);
			logger->flush();
		}
		break;

	case AbstractDevice::DEVICE_TERMINATED:
		logger->info((boost::format(gettext("Device %s is terminated.")) % message1).str());
		logger->flush();
		break;

	case AbstractDevice::DEVICE_TERMINATED_ERROR:
		logger->error((boost::format(gettext("Device %s is terminated with errors.")) % message1).str());
		logger->flush();
		break;

	default:
		break;
	}

	return true;
}

}
}
