#pragma once

#include "Model.h"
#include "ModelComponent.h"
#include "Transform3D.h"

#include "Force.h"
#include "ColliderManager.h"
#include "PhysicsWorld.h"
#include "PhysicalBody.h"
#include "Polyhedron3D.h"  
#include "Newtonian.h"
#include "Gravity.h"
#include "CollisionReaction.h"
#include "Friction.h"

#include "DirectionalLight3D.h"
#include "PointLight3D.h"

#include <cstdlib> 
#include <ctime> 
#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "Canvas.h"
#include "Image.h"
#include "Text.h"
#include "RectTransform.h"

#include "ComponentLibrary.h"

namespace SpaceRegisterComponents {
    void RegisterAllComponents() {
        ComponentRegistry::Register<Transform3D>("Transform3D");
        ComponentRegistry::Register<ModelComponent>("ModelComponent");
        ComponentRegistry::Register<PhysicalBody>("PhysicalBody");
        ComponentRegistry::Register<Polyhedron3D>("Polyhedron3D");
        ComponentRegistry::Register<Image>("Image");
        ComponentRegistry::Register<Text>("Text");
        ComponentRegistry::Register<Canvas>("Canvas"); 
        ComponentRegistry::Register<RectTransform>("RectTransform");
    }
}