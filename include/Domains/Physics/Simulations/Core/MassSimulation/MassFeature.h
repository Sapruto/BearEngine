#pragma once

#include "../../../Body/IPhysicFeature.h"
#include "Logger/Logger.h"

namespace Physic::Mass {
    class MassSimulator;

    struct MassFeature : public Body::IPhysicFeature {
    private:
        float volume = 1.0f;
        float density = 1.0f;
        float mass = 1.0f;

        bool isVolumeFromCollider = false;

        void RecomputeMass() { mass = volume * density; }

        void SetColliderVolume(float v) {
            volume = v;
            RecomputeMass();
        }

        friend class MassSimulator;

    public:
        MassFeature() { RecomputeMass(); }

        explicit MassFeature(float density_, bool fromCollider)
            : density(density_), isVolumeFromCollider(fromCollider) {
            RecomputeMass();
        }

        MassFeature(float volume_, float density_)
            : volume(volume_), density(density_) {
            RecomputeMass();
        }

        static MassFeature FromMass(float mass_, float volume_) {
            MassFeature f;
            f.volume = volume_;
            f.density = volume_ > 0.0f ? mass_ / volume_ : 0.0f;
            f.RecomputeMass();
            return f;
        }

        void SetDensity(float d) {
            density = d;
            RecomputeMass();
        }

        bool SetMass(float m) {
            if (volume <= 0.0f) {
                Logging::LoggerFacade::Warn(
                    "MassFeature::SetMass: volume is 0, cannot derive density");
                return false;
            }
            density = m / volume;
            RecomputeMass();
            return true;
        }

        bool SetVolume(float v) {
            if (isVolumeFromCollider) {
                Logging::LoggerFacade::Warn(
                    "MassFeature::SetVolume: volume is driven by collider, ignored");
                return false;
            }
            volume = v;
            RecomputeMass();
            return true;
        }

        void SetVolumeFromCollider(bool enabled) {
            isVolumeFromCollider = enabled;
        }

        bool IsVolumeFromCollider() const { return isVolumeFromCollider; }

        float GetMass() const { return mass; }
        float GetVolume() const { return volume; }
        float GetDensity() const { return density; }
    };
}