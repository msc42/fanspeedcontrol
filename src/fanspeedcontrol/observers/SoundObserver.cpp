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

#include "SoundObserver.h"

#include <chrono>
#include <cstdlib>
#include <string>

#include "fanspeedcontrol/devices/AbstractDevice.h"

namespace msc42 {
namespace fanspeedcontrol {

const std::chrono::milliseconds timeToRepeatBeep(1000);

SoundObserver::SoundObserver(bool beep, const std::string soundFile,
		const std::chrono::milliseconds &timeToStartSoundAgain)
: beep(beep), soundFile(soundFile), timeToStartSoundAgain(timeToStartSoundAgain) {
	playSoundCommand = "ffplay -loglevel panic -nodisp " + soundFile + " &";
	lastTimeBeepStarted = std::chrono::steady_clock::now() - timeToRepeatBeep - std::chrono::milliseconds(1);
}

SoundObserver::~SoundObserver() {
}

bool SoundObserver::notify(int messageId, const std::string &message1, const std::string &message2) {
	if (messageId == AbstractDevice::MODE_AUTOMATIC_SET_ERROR) {
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

		if (beep && std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimeBeepStarted)
				>= timeToRepeatBeep) {
			std::system("beep");
			lastTimeBeepStarted = now;
		}

		if (!soundFile.empty() && std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimeSoundStarted)
				>= timeToStartSoundAgain) {
			std::system(playSoundCommand.c_str());
			lastTimeSoundStarted = now;
		}
	}

	return true;
}

}
}
