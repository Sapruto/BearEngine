#pragma once

#include "EditorUI.h"

class EditorLogic {
protected:
    EditorUI* ui;

public:
    void SetUI(EditorUI* ui) { this->ui = ui; }

    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void Destroy() = 0;
};