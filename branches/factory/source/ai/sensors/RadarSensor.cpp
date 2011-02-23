#include "core/Common.h"
#include "core/IrrSerialize.h"
#include "ai/sensors/RadarSensor.h"
#include "render/LineSet.h"
#include <iostream>
#include <algorithm>

namespace OpenNero
{
    const double RadarSensor::kDistanceScalar = 1.0/10.0;

    //! Create a new RadarSensor
    //! @param radius the radius of the radar sector (how far it extends)
    //! @param types the bitmask which is used to filter objects by type
    RadarSensor::RadarSensor(
        double lb, double rb, 
        double bb, double tb, 
        double radius, U32 types,
        bool vis)
        : Sensor(1, types)
        , leftbound(LockDegreesTo180(lb))
        , rightbound(LockDegreesTo180(rb))
        , bottombound(LockDegreesTo180(bb))
        , topbound(LockDegreesTo180(tb))
        , radius(radius)
        , vis(vis)
    {
    }
    
    RadarSensor::~RadarSensor() {}

    //! Decide if this sensor is interested in a particular object
    bool RadarSensor::process(SimEntityPtr source, SimEntityPtr target)
    {
        if (observed)
        {
            observed = false;
            value = 0;
        }
        Vector3f sourcePos = source->GetPosition();
        Vector3f targetPos = target->GetPosition();
        Vector3f vecToTarget = targetPos - sourcePos;
        double distToTarget = vecToTarget.getLength();
        double myHeading = source->GetRotation().Z;
        double tgtAngle = RAD_2_DEG * atan2(vecToTarget.Y, vecToTarget.X);
        double yawToTarget = LockDegreesTo180(tgtAngle - myHeading);
        double pitchToTarget = 0; // TODO: for now
        if (distToTarget <= radius &&                                       // within radius
            ((leftbound <= yawToTarget && yawToTarget <= rightbound) ||     // yaw within L-R angle bounds
             (rightbound < leftbound) && (leftbound <= yawToTarget || yawToTarget <= rightbound)) && // possibly wrapping around
			(bottombound <= pitchToTarget && pitchToTarget <= topbound) )   // pitch within B-T angle bounds
        {
            if (vis) {
                Vector3f r = source->GetRotation();
                double h = DEG_2_RAD * myHeading;
                double y = DEG_2_RAD * yawToTarget;
                double lb = DEG_2_RAD * leftbound;
                double rb = DEG_2_RAD * rightbound;
                double c = lb + (rb - lb) / 2.0;
                Vector3f tp(sourcePos.X + distToTarget * cos(y + h), sourcePos.Y + distToTarget * sin(y + h), sourcePos.Z);
                Vector3f hp(sourcePos.X + distToTarget * cos(h), sourcePos.Y + distToTarget * sin(h), sourcePos.Z);
                Vector3f cp(sourcePos.X + distToTarget * cos(c + h), sourcePos.Y + distToTarget * sin(c + h), sourcePos.Z);
                //Vector3f hp(sourcePos.X + distToTarget * cos(h), sourcePos.Y + distToTarget * sin(h), sourcePos.Z);
                LineSet::instance().AddSegment(sourcePos, hp, SColor(255,255,255,255));
                LineSet::instance().AddSegment(sourcePos, tp, SColor(255,255,0,0));
            }
            if (distToTarget > 0) {
                value += kDistanceScalar * radius / distToTarget;
            } else {
                value += 1;
            }
            //LOG_F_DEBUG("ai.sensor", 
            //            "lb: " << leftbound << " hdg: " << r.Z << " rb: " << rightbound <<
            //            " R: " << radius <<
            //            " src: " << sourcePos << " tgt: " << targetPos <<
            //            " dir: " << vecToTarget << " dst: " << distToTarget <<
            //            " yaw: " << yawToTarget << " ptc: " << pitchToTarget <<
            //            " v: " << value);
        }
        return true;
    }
    
    //! get the minimal possible observation
    double RadarSensor::getMin()
    {
        return 0;
    }
    
    //! get the maximum possible observation
    double RadarSensor::getMax()
    {
        return 1;
    }

    //! Get the value computed for this sensor given the filtered objects
    double RadarSensor::getObservation(SimEntityPtr source)
    {
        observed = true;
        return std::max(0.0, std::min(value,1.0));
    }
    
    void RadarSensor::toXMLParams(std::ostream& out) const
    {
        Sensor::toXMLParams(out);
        out << "leftbound=\"" << leftbound << "\" "
            << "rightbound=\"" << rightbound << "\" "
            << "topbound=\"" << topbound << "\" "
            << "bottombound=\"" << bottombound << "\" "
            << "radius=\"" << radius << "\" ";
    }

    void RadarSensor::toStream(std::ostream& out) const
    {
        out << "<RadarSensor ";
        toXMLParams(out);
        out << " />";
    }
}
