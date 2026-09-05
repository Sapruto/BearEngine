#pragma once

#include <string>
#include "Component.h"

class BaseCollider;

class Listener : public Component {
private:
    float hearingStrong;
    BaseCollider* hearingCollider;

public:
    Listener(float hearingStrong = 1.0f, BaseCollider hearingCollider = nullptr);
    ~Listener();

    void Start() override;
    void Update() override;
    void Destroy() override;

    void SetHearingStrong(float newHearingStrong);
    void SetHearingCollider(BaseCollider* newHearingCollider);

    float GetHearingStrong() const;

    BaseCollider* GetBaseCollider() const;
    const BaseCollider* GetBaseCollider() const;
}