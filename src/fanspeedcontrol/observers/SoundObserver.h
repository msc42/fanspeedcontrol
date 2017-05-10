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

#ifndef FANSPEEDCONTROL_OBSERVERS_SOUNDOBSERVER_H_
#define FANSPEEDCONTROL_OBSERVERS_SOUNDOBSERVER_H_

#include <chrono>
#include <string>

#include <patterns/observer/AbstractObserver.h>

namespace msc42 {
namespace fanspeedcontrol {

class SoundObserver: public msc42::patterns::AbstractObserver {
public:
	SoundObserver(bool beep, const std::string soundFile = "", const std::chrono::milliseconds &timeToStartSoundAgain = std::chrono::milliseconds(0));
	virtual ~SoundObserver();
	virtual bool notify(int messageId, const std::string &message1 = "", const std::string &message2 = "");

private:
	const bool beep;
	const std::string soundFile;
	std::string playSoundCommand;
	const std::chrono::milliseconds timeToStartSoundAgain;
	std::chrono::steady_clock::time_point lastTimeBeepStarted;
	std::chrono::steady_clock::time_point lastTimeSoundStarted;
};

}
}

#endif /* FANSPEEDCONTROL_OBSERVERS_SOUNDOBSERVER_H_ */
