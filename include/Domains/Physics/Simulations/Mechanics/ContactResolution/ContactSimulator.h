#pragma once

#include "World/GameObject.h"
#include "Domains/Collisions/3D/Polyhedron3D.h"

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
            Speeds(Vector3f speedA_, Vector3f speedB_) : speedA(speedA_), speedB(speedB_) {}
        };

        struct InverseMasses {
            float invMassA = 1.0f;
            float invMassB = 1.0f;

            InverseMasses() = default;
            InverseMasses(float invMassA_, float invMassB_) : invMassA(invMassA_), invMassB(invMassB_) {}
        };

        std::vector<Contact> contacts;

        const float defaultRestitution = 0.3f;

        Speeds GetSpeeds(Body::PhysicalBody* a, Body::PhysicalBody* b) {
            auto kinematicA = a->GetFeatureOfType<Kinematics::KinematicFeature>();
            auto kinematicB = b->GetFeatureOfType<Kinematics::KinematicFeature>();
            if (!kinematicA || !kinematicB) return Speeds();

            return Speeds(kinematicA->velocity, kinematicB->velocity);
        }

        InverseMasses GetInverseMasses(Body::PhysicalBody* a, Body::PhysicalBody* b) {
            auto massA = a->GetFeatureOfType<Mass::MassFeature>();
            auto massB = b->GetFeatureOfType<Mass::MassFeature>();
            if (!massA || !massB) return InverseMasses();

            float invMassA = massA->GetMass() > 0.0f ? 1.0f / massA->GetMass() : 0.0f;
            float invMassB = massB->GetMass() > 0.0f ? 1.0f / massB->GetMass() : 0.0f;

            return InverseMasses(invMassA, invMassB);
        }

        float GetRestitution(Body::PhysicalBody* a, Body::PhysicalBody* b) {
            auto restitutionA = a->GetFeatureOfType<RestitutionFeature>();
            auto restitutionB = b->GetFeatureOfType<RestitutionFeature>();
            if (!restitutionA || !restitutionB) return defaultRestitution;

            return (restitutionA->restitution + restitutionB->restitution) / 2;
        }

    public:
        ContactSimulator() { SetLayer(SimulatorLayers::Mechanics::CONTACT); }

        void BeginFrameSimulation() override {
            contacts.clear();

            auto& bodyRegistry = GetBodyRegistry();

            std::vector<std::pair<Body::BodyId, Polyhedron3D*>> entries;

            for (auto body : bodyRegistry.GetAllBodies()) {
                if (!body) continue;
                auto* go = body->GetGameObject();
                if (!go) continue;

                auto* collider = go->GetComponentOfType<Polyhedron3D>();
                if (!collider) continue;
                if (!collider->IsEnabled() || collider->IsTrigger()) continue;

                entries.emplace_back(body->GetId(), collider);
            }

            for (size_t i = 0; i < entries.size(); i++) {
                auto* ca = entries[i].second;
                for (size_t j = i + 1; j < entries.size(); j++) {
                    auto* cb = entries[j].second;

                    Vector3f point, normal;
                    float penetration = 0.0f;

                    if (!ca->GetCollisionInfo(cb, point, normal, penetration)) continue;
                    if (penetration <= 0.0f) continue;

                    Contact c;
                    c.a = entries[i].first;
                    c.b = entries[j].first;
                    c.point = point;
                    c.normal = normal;
                    c.penetration = penetration;

                    contacts.push_back(c);
                }
            }
        }

        void EndFrameSimulation() override {
            auto& eventBus = GetEventBus();
            auto& bodyRegistry = GetBodyRegistry();

            for (auto& contact : contacts) {
                auto a = bodyRegistry.GetBody(contact.a);
                auto b = bodyRegistry.GetBody(contact.b);
                if (!a || !b) continue;

                auto speeds = GetSpeeds(a, b);

                auto relativeSpeed = speeds.speedB - speeds.speedA;

                auto normalSpeed = relativeSpeed.Dot(contact.normal);
                if (normalSpeed > 0.0f) continue;

                auto inverseMasses = GetInverseMasses(a, b);

                auto restitution = GetRestitution(a, b);

                auto normalImpulse = -(1 + restitution) * normalSpeed / (inverseMasses.invMassA + inverseMasses.invMassB);
                normalImpulse = fmax(normalImpulse, 0);

                eventBus.AddEvent(std::make_unique<Kinematics::ImpulseEvent>(contact.a, -(normalImpulse * inverseMasses.invMassA * contact.normal)));
                eventBus.AddEvent(std::make_unique<Kinematics::ImpulseEvent>(contact.b, normalImpulse * inverseMasses.invMassB * contact.normal));
            }
        }
    };
}