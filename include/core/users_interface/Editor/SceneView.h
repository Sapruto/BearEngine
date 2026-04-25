#pragma once

#include "Camera3D.h"
#include "ModelRenderer.h"

#include <memory>

class Editor;
class SceneCreator;

class SceneView {
private:
    Editor* editor;
    SceneCreator* creator;

    std::string path;

    std::vector<Component*> editComponents;
    
    Camera3D editorCamera;
    ModelRenderer* modelRenderer = nullptr;
    
public:
    SceneView(Editor* editor, const std::string& path, SceneCreator* sc) : editor(editor), path(path), creator(sc) {}
    
    void Start();
    void Update();
    void Render();
};