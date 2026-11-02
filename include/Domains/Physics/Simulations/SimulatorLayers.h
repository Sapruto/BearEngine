#pragma once

namespace Physic::SimulatorLayers {
    inline const unsigned int CORE = 0;

    namespace Mechanics {
        inline const unsigned int KINEMATICS = 5;
        inline const unsigned int DYNAMICS = 6;

        namespace Forces {
            inline const unsigned int GRAVITY = 7;
        }
    };

    namespace Thermodynamics {
        inline const unsigned int THERMAL = 8;
    }
}