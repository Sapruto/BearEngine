#pragma once

#include "PhysicFeature.h"

#include "Transform3D.h"
#include "Vertex3D.h"

class Liquid : public PhysicFeature{
private:
    std::vector<std::pair<Vertex3D, Vertex3D>> limiterPoints;
    std::vector<Vertex3D> spawnPoints;

    std::vector<Vertex3D> mainPoints;

    Transform3D* transform;

    bool isValid;

public:
    Liquid(const std::vector<std::pair<Vertex3D, Vertex3D>>&, const std::vector<Vertex3D>& spawnPoints);
    ~Liquid(){}

    void Initialize() override;

    void UpdateBody() override;
    void ReactionOnEvent(BasePhysicsEvent* event) override;

    void Destroy() override;

    void RefactorModel();
};