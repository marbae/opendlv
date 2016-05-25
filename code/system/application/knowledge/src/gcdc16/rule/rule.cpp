/**
 * Copyright (C) 2015 Chalmers REVERE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include <ctype.h>
#include <cstring>
#include <cmath>
#include <iostream>

#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/data/TimeStamp.h"

#include "opendlvdata/GeneratedHeaders_opendlvdata.h"

#include "gcdc16/rule/rule.hpp"

namespace opendlv {
namespace knowledge {
namespace gcdc16 {
namespace rule {

/**
* Constructor.
*
* @param a_argc Number of command line arguments.
* @param a_argv Command line arguments.
*/
Rule::Rule(int32_t const &a_argc, char **a_argv)
: DataTriggeredConferenceClientModule(
	a_argc, a_argv, "knowledge-gcdc16-rule"),
  m_object(),
  m_desiredAzimuth(0.0f)
	//standstillDistance(6), //TODO: Get actual value at GCDC in meters
	//headway(1), //TODO: Get actual value at GCDC in seconds
	//minimumEuclideanDistance(5) //TODO: Get actual value at GCDC in meters
{
}

Rule::~Rule()
{
}

/**
* Receives .
* Sends .
*/
/*
double Rule::getDistances(double hostVelocity)
{
	double desiredDistance = standstillDistance + headway * hostVelocity;
	//double safeDistance = scalingFactor * desiredDistance;
	//returnVector[0] = desiredDistance;
	//returnvector[1] = safeDistance;
	return desiredDistance;
}

bool Rule::euclideanDistance(double measuredDistance)
{
	double distanceError = measuredDistance - minimumEuclideanDistance;
	if (distanceError >= 0) {
		return true;
	}
	return false;

}
*/

void Rule::nextContainer(odcore::data::Container &a_container)
{
	if (a_container.getDataType() == opendlv::proxy::ControlState::ID()) {
    opendlv::proxy::ControlState controlState =
    a_container.getData<opendlv::proxy::ControlState>();

    if (controlState.getIsAutonomous()) {
      odcore::data::TimeStamp timestamp;
      opendlv::knowledge::Event event(timestamp, "gcdcReady");
    
      odcore::data::Container objectContainer(event);
      getConference().send(objectContainer);
    }

  } else if (a_container.getDataType() == opendlv::perception::Object::ID()) {
    opendlv::perception::Object unpackedObject =
    a_container.getData<opendlv::perception::Object>();

    int16_t id = unpackedObject.getObjectId();

    if (id != -1) {
      return;
    }

    opendlv::model::Direction direction = unpackedObject.getDirection();
    float azimuth = direction.getAzimuth();

    if (std::abs(azimuth) < 0.22f) {
      if (m_object == nullptr) {
        m_object.reset(new opendlv::perception::Object(unpackedObject));
      } else {
        if (unpackedObject.getDistance() < m_object->getDistance())
          m_object.reset(new opendlv::perception::Object(unpackedObject));
      }
    }
    m_desiredAzimuth = m_object->getDirection().getAzimuth();
    opendlv::model::Direction objectDirection(m_desiredAzimuth, 0.0f);
    opendlv::sensation::DesiredDirectionOfMovement desiredDirection(objectDirection);
    odcore::data::Container objectContainer(desiredDirection);
    getConference().send(objectContainer);
  }
}






void Rule::setUp()
{
}

void Rule::tearDown()
{
}

} // rule
} // gcdc16
} // knowledge
} // opendlv
