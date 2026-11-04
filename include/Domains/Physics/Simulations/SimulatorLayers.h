#pragma once

namespace Physic::SimulatorLayers {
    inline const unsigned int CORE = 0;

    namespace Mechanics {
        inline const unsigned int KINEMATICS = 5;
        inline const unsigned int DYNAMICS = 6;
        inline const unsigned int CONTACT = 7;

        namespace Forces {
            inline const unsigned int GRAVITY = 8;
        }
    };

    namespace Thermodynamics {
        inline const unsigned int THERMAL = 9;
    }
}