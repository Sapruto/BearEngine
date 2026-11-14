#pragma once

#include "World/GameObject.h"
#include "Domains/Collisions/Core/CollisionDomain.h"
#include "Domains/Collisions/Core/ICollisionDetector.h"
#include "Domains/Collisions/3D/CollisionDetector3D.h"
#include "Domains/Collisions/3D/Polyhedron3D.h"
#include "Domains/Collisions/Core/ShapeRegistry.h"
#include "Domains/Physics/Body/PhysicalBody.h"

#include "../../../Core/IPhysicSimulator.h"
#include "../../SimulatorLayers.h"
#include "../../Core/MassSimulation/MassFeature.h"
#include "../Kinematics/KinematicFeature.h"
#include "../Kinematics/KinematicEvent.h"
#include "Contact.h"
#include "RestitutionFeature.h"

namespace Physic::Mechanics::Contact {
    class ContactSimulator : public IPhysicSimulator {
    private:
        struct Speeds {
            Vector3f speedA = Vector3f::Zero();
            Vector3f speedB = Vector3f::Zero();
            Speeds() = default;
            Speeds(Vector3f a, Vector3f b) : speedA(a), speedB(b) {}
        };

        struct InverseMasses {
            float invMassA = 0.0f;
            float invMassB = 0.0f;
            InverseMasses() = default;
            InverseMasses(float a, float b) : invMassA(a), invMassB(b) {}
        };

        std::vector<Contact> contacts;
        CollisionDomain* collisionDomain = nullptr;

        const float defaultRestitution = 0.3f;
        const float positionCorrectionPercent = 0.8f;
        const float slop = 0.001f;

        Speeds GetSpeeds(Body::PhysicalBody* a, Body::PhysicalBody* b) {
            auto* ka = a->GetFeatureOfType<Kinematics::KinematicFeature>();
            auto* kb = b->GetFeatureOfType<Kinematics::KinematicFeature>();
            if (!ka || !kb) return Speeds();
            return Speeds(ka->velocity, kb->velocity);
        }

        InverseMasses GetInverseMasses(Body::PhysicalBody* a, Body::PhysicalBody* b) {
            auto* ma = a->GetFeatureOfType<Mass::MassFeature>();
            auto* mb = b->GetFeatureOfType<Mass::MassFeature>();
            float invA = (ma && ma->GetMass() > 0.0f) ? 1.0f / ma->GetMass() : 0.0f;
            float invB = (mb && mb->GetMass() > 0.0f) ? 1.0f / mb->GetMass() : 0.0f;
            return InverseMasses(invA, invB);
        }

        float GetRestitution(Body::PhysicalBody* a, Body::PhysicalBody* b) {
            auto* ra = a->GetFeatureOfType<RestitutionFeature>();
            auto* rb = b->GetFeatureOfType<RestitutionFeature>();
            if (!ra || !rb) return defaultRestitution;
            return (ra->restitution + rb->restitution) * 0.5f;
        }

        static Body::PhysicalBody* BodyFromShape(IShape3D* shape) {
            if (!shape) return nullptr;
            auto* go = shape->GetGameObject();
            if (!go) return nullptr;
            return go->GetComponentOfType<Body::PhysicalBody>();
        }

    public:
        ContactSimulator() { SetLayer(SimulatorLayers::Mechanics::CONTACT); }

        void SetCollisionDomain(CollisionDomain* domain) { collisionDomain = domain; }

        void BeginFrameSimulation() override {
            contacts.clear();
            if (!collisionDomain) return;

            auto* detector = collisionDomain->GetDetector();
            if (!detector) return;

            const auto& raw = detector->GetCollisions();

            contacts.reserve(raw.size());

            for (const auto& info : raw) {
                Body::PhysicalBody* a = BodyFromShape(info.a);
                Body::PhysicalBody* b = BodyFromShape(info.b);
                if (!a || !b) continue;
                if (a == b) continue;

                if (!a->GetFeatureOfType<Mass::MassFeature>()) continue;
                if (!b->GetFeatureOfType<Mass::MassFeature>()) continue;

                const float depth = info.penetration.Magnitude();
                if (depth <= 0.0f) continue;

                Vector3f normal = info.penetration / depth;
                normal = -normal;

                Contact c;
                c.a = a->GetId();
                c.b = b->GetId();
                c.point = (info.a->GetGlobalCenter() + info.b->GetGlobalCenter()) * 0.5f;
                c.normal = normal;
                c.penetration = depth;
                contacts.push_back(c);
            }
        }

        void EndFrameSimulation() override {
            auto& eventBus = GetEventBus();
            auto& bodyRegistry = GetBodyRegistry();

            for (auto& contact : contacts) {
                auto* a = bodyRegistry.GetBody(contact.a);
                auto* b = bodyRegistry.GetBody(contact.b);
                if (!a || !b) continue;

                const auto inv = GetInverseMasses(a, b);
                const float invSum = inv.invMassA + inv.invMassB;
                if (invSum <= 1e-8f) continue;

                const auto speeds = GetSpeeds(a, b);
                const Vector3f relativeSpeed = speeds.speedB - speeds.speedA;
                const float normalSpeed = relativeSpeed.Dot(contact.normal);

                const float correction = std::max(contact.penetration - slop, 0.0f)
                                       * positionCorrectionPercent / invSum;
                const Vector3f correctionVec = contact.normal * correction;

                if (auto* ta = a->GetFeatureOfType<Physic::Transform::TransformFeature>()) {
                    ta->position -= correctionVec * inv.invMassA;
                }
                if (auto* tb = b->GetFeatureOfType<Physic::Transform::TransformFeature>()) {
                    tb->position += correctionVec * inv.invMassB;
                }

                if (normalSpeed > 0.0f) continue;

                const float restitution = GetRestitution(a, b);
                float normalImpulse = -(1.0f + restitution) * normalSpeed / invSum;
                if (normalImpulse < 0.0f) normalImpulse = 0.0f;

                eventBus.AddEvent(std::make_unique<Kinematics::ImpulseEvent>(
                    contact.a, -(normalImpulse * inv.invMassA * contact.normal)));
                eventBus.AddEvent(std::make_unique<Kinematics::ImpulseEvent>(
                    contact.b, (normalImpulse * inv.invMassB * contact.normal)));
            }
        }
    };
}