/* ============================================================
 *
 * This file is part of the rsb-spread project.
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 * Copyright (C) 2012-2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the ``LGPL''),
 * or (at your option) any later version.
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the LGPL along with this
 * program. If not, go to http://www.gnu.org/licenses/lgpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The development of this software was supported by:
 *   CoR-Lab, Research Institute for Cognition and Robotics
 *     Bielefeld University
 *
 * ============================================================ */

#include "InPullConnector.h"

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;

using namespace rsb::converter;

namespace rsb {
namespace transport {
namespace spread {

InPullConnector::InPullConnector(ConverterSelectionStrategyPtr converters,
                                 SpreadConnectionPtr           connection) :
    ConverterSelectingConnector<std::string>(converters),
    ConnectorBase(connection),
    InConnector(converters, connection),
    logger(Logger::getLogger("rsb.transport.spread.InPullConnector")) {
}

InPullConnector::~InPullConnector() {
    if (this->active) {
        deactivate();
    }
}

void InPullConnector::activate() {
    InConnector::activate();

    this->active = true;
}

void InPullConnector::deactivate() {
    InConnector::deactivate();

    this->active = false;
}

void InPullConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec& /*specs*/) {
}

EventPtr InPullConnector::raiseEvent(bool block) {
    assert(block);

    SpreadMessage message;
    rsb::protocol::NotificationPtr notification;
    EventPtr event;
    while (true) {
        try {
            this->connection->receive(message);

            notification = this->messageHandler.handleMessage(message);
        } catch (const std::exception& exception) {
            handleError("receiving notification", exception,
                        "Skipping notification", "Terminating");
        }
        if (!notification) {
            continue;
        }

        EventPtr event = notificationToEvent(notification);
        if (!event) {
            continue;
        }

        return event;
    };
    // This should never happen so far unless non-blocking (not implemented so far)
    return EventPtr();
}

}
}
}
