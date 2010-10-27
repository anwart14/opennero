#ifndef _OPENNERO_AI_SENSORS_SENSOR_ARRAY_H_
#define _OPENNERO_AI_SENSORS_SENSOR_ARRAY_H_

#include "ai/sensors/Sensor.h"
#include "core/Common.h"
#include "game/SimEntity.h"
#include "game/objects/TemplatedObject.h"
#include "game/objects/SimEntityComponent.h"
#include "ai/AI.h"

namespace OpenNero {

    BOOST_SHARED_DECL(SensorArray);
    
    class SensorArray 
        : public SimEntityComponent
    {
        std::vector<SensorPtr> sensors;
    public:
        SensorArray() : SimEntityComponent(SimEntityPtr()) {};
        SensorArray(SimEntityPtr parent) : SimEntityComponent(parent) {}
        virtual bool LoadFromTemplate(ObjectTemplatePtr t, const SimEntityData& data) { return true; }
        size_t getNumSensors() { return sensors.size(); }
        void addSensor(SensorPtr sensor) { sensors.push_back(sensor); }
        Observations getObservations();
        friend std::ostream& operator<<(std::ostream& out, const SensorArray& sa);
    };

}

#endif // _OPENNERO_AI_SENSORS_SENSOR_ARRAY_H_