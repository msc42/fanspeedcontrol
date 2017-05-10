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

#include "Observable.h"

#include <memory>
#include <vector>

#include "AbstractObserver.h"

namespace msc42 {
namespace patterns {

Observable::Observable() {
}

Observable::~Observable() {
}

void Observable::registerObserver(std::shared_ptr<AbstractObserver> observer) {
	observers.push_back(observer);
}

void Observable::notifyObservers(int messageId, const std::string &message1, const std::string &message2) const {
	for (const std::shared_ptr<AbstractObserver> &observer : observers) {
		observer->notify(messageId, message1, message2);
	}
}

}
}

