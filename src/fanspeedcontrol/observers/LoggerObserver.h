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

#ifndef FANSPEEDCONTROL_OBSERVERS_LOGGEROBSERVER_H_
#define FANSPEEDCONTROL_OBSERVERS_LOGGEROBSERVER_H_

#include <chrono>
#include <memory>

#include <spdlog/spdlog.h>

#include "patterns/observer/AbstractObserver.h"

namespace msc42 {
namespace fanspeedcontrol {

class Observable;

class LoggerObserver: public msc42::patterns::AbstractObserver {
public:
	LoggerObserver(const std::chrono::milliseconds &timeToLogRepeatedError, const std::string &logLevel,
			const std::string &appName, const std::string &logFile);
	virtual ~LoggerObserver();
	bool notify(int messageId, const std::string &message1 = "", const std::string &message2 = "");

private:
	std::shared_ptr<spdlog::logger> logger;

	const std::chrono::milliseconds timeToLogRepeatedError;
	std::chrono::steady_clock::time_point lastLoggedTemperatureError;
	std::chrono::steady_clock::time_point lastLoggedModeAutomaticSet;
	std::chrono::steady_clock::time_point lastLoggedModeAutomaticSetError;
	std::chrono::steady_clock::time_point lastLoggedModeManualSetError;
	std::chrono::steady_clock::time_point lastLoggedFanSetError;
	std::chrono::steady_clock::time_point lastLoggedTemperatureWarn;
};

}
}

#endif /* FANSPEEDCONTROL_OBSERVERS_LOGGEROBSERVER_H_ */
