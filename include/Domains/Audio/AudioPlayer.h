#pragma once

#include <string>

#include "Component.h"

class BaseCollider;

class AudioPlayer : public Component {
private:
    float audioStrong;
    BaseCollider* playerCollider;

    Audio* audio;

public:
    AudioPlayer(const std::string& audioPath = "");
    ~AudioPlayer();

    void Start() override;
    void Update() override;
    void Destroy() override;

    void SetAudio(Audio* audio);
    void SetAudio(const std::string& audioPath = "");
    
    void SetAudioStrong(float newHearingStrong);
    void SetCollider(BaseCollider* newHearingCollider);

    Audio* GetAudio() const;
    const Audio* GetAudio() const;

    float GetAudioStrong() const;

    BaseCollider* GetBaseCollider() const;
    const BaseCollider* GetBaseCollider() const;
}