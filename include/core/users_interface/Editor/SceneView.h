#pragma once

#include "Renderer.h"

class SceneView{
private:
    Editor* editor;
    
    std::sting path;

public:
    SceneView(Editor* editor, const std::string& path) : editor(editor), path(path) {}

    void Update();
};