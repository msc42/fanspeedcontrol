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

#include "NotifyObserver.h"

#include <chrono>
#include <string>

#include <boost/format.hpp>
#include <libnotify/notify.h>

#include "fanspeedcontrol/config/ArgsAndConfigProcessor.h"
#include "fanspeedcontrol/devices/AbstractDevice.h"
#include "SharedStrings.h"

namespace msc42 {
namespace fanspeedcontrol {

NotifyObserver::NotifyObserver(const std::chrono::milliseconds &timeToNotifyRepeatedError, const std::string &appName)
: timeToNotifyRepeatedError(timeToNotifyRepeatedError) {
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	lastNotifiedTemperatureError = now - timeToNotifyRepeatedError - std::chrono::milliseconds(1);
	lastNotifiedModeAutomaticSet = now - timeToNotifyRepeatedError - std::chrono::milliseconds(1);
	lastNotifiedModeAutomaticSetError = now - timeToNotifyRepeatedError - std::chrono::milliseconds(1);
	lastNotifiedModeManualSetError = now - timeToNotifyRepeatedError - std::chrono::milliseconds(1);
	lastNotifiedFanSetError = now - timeToNotifyRepeatedError - std::chrono::milliseconds(1);
	lastNotifiedTemperaturWarn = now - timeToNotifyRepeatedError - std::chrono::milliseconds(1);

	notify_init(appName.c_str());
}

NotifyObserver::~NotifyObserver() {
}


void newMessage(const std::string &message) {
	NotifyNotification* n = notify_notification_new(APP_NAME.c_str(), message.c_str(), 0);
	notify_notification_set_timeout(n, NOTIFY_EXPIRES_NEVER);
	notify_notification_set_urgency(n, NOTIFY_URGENCY_CRITICAL);
	notify_notification_show(n, 0);
}

bool NotifyObserver::notify(int messageId, const std::string &message1, const std::string &message2) {
	std::chrono::steady_clock::time_point now;

	switch (messageId) {

	case AbstractDevice::CONFIG_FILE_ERROR:
		newMessage(CONFIG_FILE_ERROR_MESSAGE);
		break;

	case AbstractDevice::TEMPERATUR_READ_ERROR:
		now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastNotifiedTemperatureError)
				>= timeToNotifyRepeatedError) {
			lastNotifiedTemperatureError = now;
			newMessage(READ_TEMPERATURE_ERROR_MESSAGE);
		}
		break;

	case AbstractDevice::MODE_AUTOMATIC_SET:
		now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastNotifiedModeAutomaticSet)
				>= timeToNotifyRepeatedError) {
			lastNotifiedModeAutomaticSet = now;
			newMessage(MODE_AUTOMATIC_SET_MESSAGE);
		}
		break;

	case AbstractDevice::MODE_AUTOMATIC_SET_ERROR:
		now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastNotifiedModeAutomaticSet)
				>= timeToNotifyRepeatedError) {
			lastNotifiedModeAutomaticSet = now;
			newMessage(MODE_AUTOMATIC_ERROR_MESSAGE);
		}
		break;

	case AbstractDevice::MODE_MANUAL_SET_ERROR:
		now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastNotifiedModeManualSetError)
				>= timeToNotifyRepeatedError) {
			lastNotifiedModeManualSetError = now;
			newMessage((MODE_MANUAL_ERROR_MESSAGE));
		}
		break;

	case AbstractDevice::FAN_SET_ERROR:
		now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastNotifiedFanSetError)
				>= timeToNotifyRepeatedError) {
			lastNotifiedFanSetError = now;
			newMessage(FAN_SET_ERROR_MESSAGE);
		}
		break;

	case AbstractDevice::TEMPERATURE_WARN:
		now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastNotifiedTemperaturWarn)
				>= timeToNotifyRepeatedError) {
			lastNotifiedTemperaturWarn = now;
			newMessage(TEMPERATURE_TOO_HIGH);
		}
		break;

	default:
		break;
	}

	return true;
}

}
}
