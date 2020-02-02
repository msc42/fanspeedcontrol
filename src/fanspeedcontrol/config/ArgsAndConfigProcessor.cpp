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

#include "ArgsAndConfigProcessor.h"

#include <array>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/program_options.hpp>
#include <json.hpp>
#include <libintl.h>

#include "fanspeedcontrol/devices/AbstractDevice.h"
#include "fanspeedcontrol/devices/NvidiaGpu.h"
#include "fanspeedcontrol/observers/LoggerObserver.h"
#include "fanspeedcontrol/observers/NotifyObserver.h"
#include "fanspeedcontrol/observers/SoundObserver.h"
#include "patterns/observer/AbstractObserver.h"

#ifndef CONFIG_FILE
#define CONFIG_FILE "/usr/local/etc/fanspeedcontrol.json"
#endif

#ifndef LOCALE_DIR
#define LOCALE_DIR "/usr/local/share/locale"
#endif

namespace msc42 {
namespace fanspeedcontrol {

const int DEFAULT_WARN = 100;
const int DEFAULT_HYSTERESIS = 0;

const std::string TYPE_KEY = "type";
const std::string ID_KEY = "id";
const std::string DISPLAY_NAME_KEY = "displayName";
const std::string HYSTERESIS_KEY = "hysteresis";
const std::string WARN_KEY = "warn";
const std::string DEVICES_ARRAY_KEY = "devices";
const std::string DEFAULT_HYSTERESIS_KEY = "defaultHysteresis";
const std::string DEFAULT_WARN_KEY = "defaultWarn";

const std::string TYPE_NVIDIA = "nvidia";

const std::string argumentHelp("help");
const std::string argumentsHelp = argumentHelp + ",h";

const std::string argumentHelpConfiguration("help-configuration");
const std::string argumentsHelpConfiguration = argumentHelpConfiguration + ",f";

const std::string argumentConfigurationPath("configuration");
const std::string argumentsConfigurationPath = argumentConfigurationPath + ",c";

const std::string argumentInterval("interval");
const std::string argumentsInterval = argumentInterval + ",i";

const std::string argumentNotifyInterval("notify-interval");
const std::string argumentsNotifyInterval = argumentNotifyInterval + ",n";

const std::string argumentLogInterval("log-interval");
const std::string argumentsLogInterval = argumentLogInterval + ",r";

const std::string argumentLogPath("log-path");
const std::string argumentsLogPath = argumentLogPath + ",p";

const std::string argumentLogLevel("log-level");
const std::string argumentsLogLevel = argumentLogLevel + ",l";

const std::string argumentBeep("beep");
const std::string argumentsBeep = argumentBeep + ",b";

const std::string argumentSoundFile("sound-file");
const std::string argumentsSoundFile = argumentSoundFile + ",s";

const std::string argumentBeginOverSound("begin-over-sound");
const std::string argumentsBeginOverSound = argumentBeginOverSound + ",o";

const std::string argumentRemoveLock("remove-lock");

nlohmann::json getExampleSingleDeviceConfig(int id = 0) {
	nlohmann::json json;
	json[TYPE_KEY] = TYPE_NVIDIA;
	json[ID_KEY] = id;
	json[DISPLAY_NAME_KEY] = ":1";
	json[HYSTERESIS_KEY] = 5;
	json[WARN_KEY] = 85;
	json["20"] = 0;
	json["40"] = 25;
	json["60"] = 40;
	json["75"] = 60;
	json["80"] = 80;
	json["85"] = 99;
	json["90"] = 100;
	return json;
}

nlohmann::json getExampleMultiDeviceConfig() {
	std::array<nlohmann::json, 2> deviceArray {{getExampleSingleDeviceConfig(0), getExampleSingleDeviceConfig(1)}};

	nlohmann::json json;
	json[DEVICES_ARRAY_KEY] = nlohmann::json(deviceArray);
	json[DEFAULT_HYSTERESIS_KEY] = 5;
	json[DEFAULT_WARN_KEY] = 85;
	return json;
}

template <typename type> type getJsonOrDefault(
		const nlohmann::json &json, const std::string &key, const type &defaultValue) {
	nlohmann::json::const_iterator keyIterator = json.find(key);
	if (keyIterator != json.end()) {
		return keyIterator.value();
	}
	return defaultValue;
}

bool isKeyThere(const nlohmann::json &json, const std::string &key) {
	if (json.find(key) != json.end()) {
		return true;
	}
	return false;
}

std::unique_ptr<AbstractDevice> getDeviceOptional(
		const nlohmann::json &deviceConfiguration, int defaultHysteresis, int defaultWarn) {
	int hysteresis = getJsonOrDefault<int>(deviceConfiguration, HYSTERESIS_KEY, defaultHysteresis);
	int warn = getJsonOrDefault<int>(deviceConfiguration, WARN_KEY, defaultWarn);

	if (!isKeyThere(deviceConfiguration, TYPE_KEY) || !isKeyThere(deviceConfiguration, ID_KEY)) {
		return std::unique_ptr<AbstractDevice>();
	}

	std::map<int, int> pairs;
	for (auto it = deviceConfiguration.begin(); it != deviceConfiguration.end(); ++it) {
		if (std::regex_match(it.key(), REGEX_IS_INTEGER)) {
			pairs[std::stoi(it.key())] = it.value();
		}
	}

	nlohmann::json::const_iterator idIterator = deviceConfiguration.find(ID_KEY);
	if (idIterator == deviceConfiguration.end()) {
		return std::unique_ptr<AbstractDevice>();
	}

	int id = idIterator.value();

	std::string type = deviceConfiguration.find(TYPE_KEY).value();
	if (type == TYPE_NVIDIA) {
		nlohmann::json::const_iterator displayNameIterator = deviceConfiguration.find(DISPLAY_NAME_KEY);
		if (displayNameIterator == deviceConfiguration.end()) {
			return std::unique_ptr<AbstractDevice>();
		}
		std::string displayName = displayNameIterator.value();
		return std::unique_ptr<AbstractDevice>(new NvidiaGpu(id, hysteresis, warn, pairs, displayName));
	} else {
		return std::unique_ptr<AbstractDevice>();
	}

	return std::unique_ptr<AbstractDevice>();
}

std::vector<std::unique_ptr<AbstractDevice>> getDevicesOptional(const std::string &file) {
	std::ifstream fileStream(file);
	nlohmann::json json;

	try {
		fileStream >> json;
	} catch(nlohmann::json::parse_error &e) {
		return std::vector<std::unique_ptr<AbstractDevice>>();
	}

	int defaultHysteresis = getJsonOrDefault<int>(json, DEFAULT_HYSTERESIS_KEY, DEFAULT_HYSTERESIS);
	int defaultWarn = getJsonOrDefault<int>(json, DEFAULT_WARN_KEY, DEFAULT_WARN);

	std::vector<std::unique_ptr<AbstractDevice>> devices;
	if (isKeyThere(json, DEVICES_ARRAY_KEY)) {
		nlohmann::json deviceArray = json[DEVICES_ARRAY_KEY];

		for (const nlohmann::json& jsonDevice : deviceArray) {
			std::unique_ptr<AbstractDevice> device = getDeviceOptional(jsonDevice, defaultHysteresis, defaultWarn);
			if (device) {
				devices.push_back(std::move(device));
			} else {
				return std::vector<std::unique_ptr<AbstractDevice>>();
			}
		}

	} else {
		std::unique_ptr<AbstractDevice> device = getDeviceOptional(json, defaultHysteresis, defaultWarn);
		if (device) {
			devices.push_back(std::move(device));
		} else {
			return std::vector<std::unique_ptr<AbstractDevice>>();
		}
	}

	return devices;
}

void setLocale() {
	setlocale(LC_ALL, "");
	bindtextdomain(APP_NAME.c_str(), LOCALE_DIR);
	textdomain(APP_NAME.c_str());
}

boost::program_options::options_description generateOptionDescription() {
	boost::program_options::options_description optionDescription(gettext("fanspeedcontrol made by Stefan Constantin "
			"and licensed under the GPLv3\n"
			"An application to control the fan speeds of supported devices.\n"
			"This application is WITHOUT ANY WARRANTY; without even the implied warranty of "
			"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. You use this application at your own risk.\n"
			"Do not use kill to terminate this application, only use sigterm, because only then fanspeedcontrol "
			"is able to set devices to automatic fan speed mode which prohibits overheating.\n"
			"Allowed options"));
	optionDescription.add_options()
		(argumentsHelp.c_str(), gettext("display help"))

		(argumentsHelpConfiguration.c_str(), gettext("display format of the configuration file"))

		(argumentsConfigurationPath.c_str(),boost::program_options::value<std::string>()
			->value_name(gettext("FILE"))->default_value(CONFIG_FILE),
			gettext("location of the configuration file"))

		(argumentsInterval.c_str(), boost::program_options::value<int>()->value_name(gettext("INTERVAL"))
			->default_value(500), gettext("polling interval in milliseconds"))

		(argumentsNotifyInterval.c_str(), boost::program_options::value<int>()
				->value_name(gettext("INTERVAL"))->default_value(60),
				gettext("minimal interval to notify repeatedly already occurred error messages in seconds"))

		(argumentsLogInterval.c_str(), boost::program_options::value<int>()->value_name(gettext("INTERVAL"))
			->default_value(60),
			gettext("minimal interval to log repeatedly already occurred error messages in seconds"))

		(argumentsLogPath.c_str(), boost::program_options::value<std::string>()->value_name(gettext("PATH"))
				->default_value(""), gettext("path of an optional log file"))

		(argumentsLogLevel.c_str(), boost::program_options::value<std::string>()->value_name(gettext("LEVEL"))
			->default_value("info"), gettext("log level, possible levels: debug, info and error"))

		(argumentsBeep.c_str(),
			gettext("call the program beep in critical states"))

		(argumentsSoundFile.c_str(), boost::program_options::value<std::string>()->value_name(gettext("FILE"))
				->default_value(""),
			gettext("this file is played with the application ffplay in critical states"))

		(argumentsBeginOverSound.c_str(), boost::program_options::value<int>()->value_name(gettext("INTERVAL"))
				->default_value(300),
			gettext("minimal interval to begin over playing the sound file with the application ffplay in seconds"))

		(argumentRemoveLock.c_str(),
			gettext("option for experts, remove the lock, use the option only if the lock is set, "
			"but no other fanspeedcontrol instance is running, in doubt restart your machine "
			"rather than remove lock, wrong usage of this option can overheat your system"));

	return optionDescription;
}

void printOnCoutConfigurationFormat() {
	std::cout << gettext(
				"The configuration file must be in the JSON format and has the following structure for a single "
				"device configuration:\n"
				"required attributes: type (value: \"nvidia\" (support must be activated in the Nvidia driver configuration)), id (value: <id of the device as integer>), "
				"displayName (value: <display name of x server connected to the device as string>)\n"
				"optional attributes: hysteresis (value: <hysteresis in celsius as integer>, warn (value: <warn temperature in celsius "
				"as integer>), "
				"arbitrary number of attributes <temperature in celsius as integer> (value: <fan speed in percent as integer>) \n"
				"\n"
				"example single device JSON file:\n")
				<< getExampleSingleDeviceConfig().dump(4) << "\n\n" << gettext(
				"The following structure is for a multi device configuration:\n"
				"required attributes: deviceArray (value: array with JSON objects described for the single device configuration)\n"
				"optional attributes: defaultHysteresis (value: <default hysteresis in celsius as integer>), defaultWarn (value: <default warn temperature in celsius as integer>)\n"
				"\n"
				"example multi device JSON file:\n")
				<< getExampleMultiDeviceConfig().dump(4) << std::endl;
}

bool removeLockAfterPositiveDialog() {
	std::cout << gettext("Are you sure to remove the lock?\n"
				"This is an option for experts. Wrong usage of the option remove-lock can overheat your system. "
				"See --help for more information.\n"
				"Type y or yes to remove the lock,  n or no to not remove the lock and then enter.\n");

	char decision;
	std::cin >> decision;

	if (decision != std::string(gettext("yes")).at(0)) {
		return false;
	}

	boost::interprocess::named_mutex::remove(DOMAIN_NAME.c_str());

	return true;
}

configuration processArguments(int argc, char *argv[], int &errorCode) {
	boost::program_options::options_description optionDescription = generateOptionDescription();

	boost::program_options::variables_map vm;

	try {
		boost::program_options::store(boost::program_options::command_line_parser(argc, argv).
				options(optionDescription).style(boost::program_options::command_line_style::unix_style).run(), vm);
	} catch (boost::program_options::error &e) {
		std::cout << gettext("The command line parameters are not valid.\n"
				"Please use the option --help to display valid command line parameters.") << std::endl;
		errorCode = EXIT_FAILURE;
		configuration emptyConfiguration;
		return emptyConfiguration;
	}

	boost::program_options::notify(vm);

	if (vm.count(argumentHelp)) {
		std::cout << optionDescription << std::endl;
		errorCode = EXIT_SUCCESS;
		configuration emptyConfiguration;
		return emptyConfiguration;
	}

	if (vm.count(argumentHelpConfiguration)) {
		printOnCoutConfigurationFormat();
		errorCode = EXIT_SUCCESS;
		configuration emptyConfiguration;
		return emptyConfiguration;
	}

	if (vm.count(argumentRemoveLock)) {
		if (!removeLockAfterPositiveDialog()) {
			errorCode = EXIT_SUCCESS;
			configuration emptyConfiguration;
			return emptyConfiguration;
		}
	}

	std::shared_ptr<LoggerObserver>	loggerObserver(new LoggerObserver(
			std::chrono::milliseconds(std::chrono::seconds(vm[argumentLogInterval].as<int>())),
			vm[argumentLogLevel].as<std::string>(), APP_NAME, vm[argumentLogPath].as<std::string>()));

	std::shared_ptr<NotifyObserver> notifyObserver(new NotifyObserver(
			std::chrono::milliseconds(std::chrono::seconds(vm[argumentNotifyInterval].as<int>())), APP_NAME));

	std::shared_ptr<SoundObserver> soundObserver(new SoundObserver(vm.count(argumentBeep),
			vm[argumentSoundFile].as<std::string>(),
			std::chrono::milliseconds(std::chrono::seconds(vm[argumentBeginOverSound].as<int>()))));

	std::vector<std::unique_ptr<AbstractDevice>> devices =
			getDevicesOptional(vm[argumentConfigurationPath].as<std::string>());

	if (devices.empty()) {
		loggerObserver->notify(AbstractDevice::CONFIG_FILE_ERROR);
		errorCode = EXIT_FAILURE;
		configuration emptyConfiguration;
		return emptyConfiguration;
	}

	for (const std::unique_ptr<AbstractDevice> &device : devices) {
		if (!device->checkIfValid()) {
			loggerObserver->notify(AbstractDevice::CONFIG_FILE_ERROR);
			errorCode = EXIT_FAILURE;
			configuration emptyConfiguration;
			return emptyConfiguration;
		}
	}

	for (const std::unique_ptr<AbstractDevice> &device : devices) {
		device->registerObserver(loggerObserver);
		device->registerObserver(notifyObserver);
		device->registerObserver(soundObserver);
	}

	const int interval = vm[argumentInterval].as<int>();

	configuration configuration;
	configuration.devices = std::move(devices);
	configuration.interval = std::chrono::milliseconds(interval);
	errorCode = EXIT_SUCCESS;
	return configuration;
}

}
}
