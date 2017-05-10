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

#ifndef FANSPEEDCONTROL_OBSERVERS_NOTIFYOBSERVER_H_
#define FANSPEEDCONTROL_OBSERVERS_NOTIFYOBSERVER_H_

#include <chrono>
#include <string>

#include "patterns/observer/AbstractObserver.h"

namespace msc42 {
namespace fanspeedcontrol {

class NotifyObserver: public msc42::patterns::AbstractObserver {
public:
	NotifyObserver(const std::chrono::milliseconds &timeToNotifyRepeatedError, const std::string &appName);
	virtual ~NotifyObserver();
	virtual bool notify(int messageId, const std::string &message1 = "", const std::string &message2 = "");

private:
	const std::chrono::milliseconds timeToNotifyRepeatedError;
	std::chrono::steady_clock::time_point lastNotifiedTemperatureError;
	std::chrono::steady_clock::time_point lastNotifiedModeAutomaticSet;
	std::chrono::steady_clock::time_point lastNotifiedModeAutomaticSetError;
	std::chrono::steady_clock::time_point lastNotifiedModeManualSetError;
	std::chrono::steady_clock::time_point lastNotifiedFanSetError;
	std::chrono::steady_clock::time_point lastNotifiedTemperaturWarn;
};

}
}

#endif /* FANSPEEDCONTROL_OBSERVERS_NOTIFYOBSERVER_H_ */
