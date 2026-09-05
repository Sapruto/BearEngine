/*#include "InspectorUI.h"

InspectorUI::InspectorUI(Canvas* cv, Scene* scn, ResourceManager* resMgr) {
    canvas = cv;
    scene = scn;
    
    font = resMgr->GetResourceAs<Font>("Assets/Base/fonts/arial.ttf");
    buttonTex = resMgr->GetResourceAs<Texture>("Assets/Base/ui/button.png");
    inputBgTex = resMgr->GetResourceAs<Texture>("Assets/Base/ui/input_bg.png");
    componentBgTex = resMgr->GetResourceAs<Texture>("Assets/Base/ui/component_bg.png");
}

void InspectorUI::Start() {}

void InspectorUI::Update() {}

void InspectorUI::Destroy() {
    for (auto& [obj, data] : objectsToDrawData) {
        if (!obj) continue;
        for (Component* comp : obj->GetComponents()) {
            if (Image* img = dynamic_cast<Image*>(comp)) canvas->RemoveUIElement(img);
            if (Text* txt = dynamic_cast<Text*>(comp)) canvas->RemoveUIElement(txt);
            if (Button* btn = dynamic_cast<Button*>(comp)) btn->ClearAllSubscriptions();
        }
        scene->RemoveGameObject(obj);
    }
    objectsToDrawData.clear();
}

void InspectorUI::DrawObject(GameObject* obj, BaseDrawData& baseData) {
    auto* data = dynamic_cast<InspectorDrawData*>(&baseData);
    if (!data || !data->inspectedObject) return;
    
    GameObject* headerGO = scene->CreateGameObject();
    obj->AddComponent<Tag>(AllTags::InspectorUI);
    
    Image* headerBg = UtilitUI::AddImage(headerGO, buttonTex,
        Vector2(basePosition.x - 300, basePosition.y + yOffsetHeader), Vector2(400, 40),
        baseAnchorMin, baseAnchorMax,
        glm::vec4(0.3f, 0.3f, 0.35f, 1.0f));
    headerBg->SetLayer(UtilitUI::Layers::InspectorButton);
    canvas->AddUIElement(headerBg);
    
    Text* headerText = UtilitUI::AddTextWithAnchors(headerGO, font,
        "Inspector: " + data->header.objectName,
        Vector2(basePosition.x - 300, basePosition.y + yOffsetHeader + 5), Vector2(380, 30),
        baseAnchorMin, baseAnchorMax,
        glm::vec4(1.0f));
    headerText->SetLayer(UtilitUI::Layers::InspectorText);
    canvas->AddUIElement(headerText);
    
    GameObject* addBtnGO = scene->CreateGameObject();
    
    Image* addBtnBg = UtilitUI::AddImage(addBtnGO, buttonTex,
        Vector2(basePosition.x - 300, basePosition.y + yOffsetAddButton), Vector2(150, 30),
        baseAnchorMin, baseAnchorMax,
        glm::vec4(0.2f, 0.6f, 0.2f, 1.0f));
    addBtnBg->SetLayer(UtilitUI::Layers::InspectorButton);
    canvas->AddUIElement(addBtnBg);
    
    Text* addBtnText = UtilitUI::AddTextWithAnchors(addBtnGO, font, "+ Add Component",
        Vector2(basePosition.x - 300, basePosition.y + yOffsetAddButton + 5), Vector2(140, 25),
        baseAnchorMin, baseAnchorMax,
        glm::vec4(1.0f));
    addBtnText->SetLayer(UtilitUI::Layers::InspectorText);
    canvas->AddUIElement(addBtnText);
    
    Button* addButton = addBtnGO->AddComponent<Button>();
    addButton->SetUIElement(addBtnBg);
    addButton->SetCanvas(canvas);
    addButton->Subscribe(Button::ClickState::JustPressed, [data]() {
        if (data->addButton.onClick) data->addButton.onClick();
    });
    
    if (data->showComponentList) {
        for (size_t i = 0; i < data->availableComponents.size(); i++) {
            GameObject* compBtnGO = scene->CreateGameObject();
            
            Image* btnBg = UtilitUI::AddImage(compBtnGO, buttonTex,
                Vector2(basePosition.x - 150, basePosition.y + yOffsetComponentList + i * 40), Vector2(300, 35),
                baseAnchorMin, baseAnchorMax,
                glm::vec4(0.3f, 0.3f, 0.4f, 1.0f));
            btnBg->SetLayer(UtilitUI::Layers::InspectorButton);
            canvas->AddUIElement(btnBg);
            
            Text* btnText = UtilitUI::AddTextWithAnchors(compBtnGO, font, data->availableComponents[i],
                Vector2(basePosition.x - 140, basePosition.y + yOffsetComponentList + i * 40 + 8), Vector2(280, 25),
                baseAnchorMin, baseAnchorMax,
                glm::vec4(1.0f));
            btnText->SetLayer(UtilitUI::Layers::InspectorText);
            canvas->AddUIElement(btnText);
            
            Button* button = compBtnGO->AddComponent<Button>();
            button->SetUIElement(btnBg);
            button->SetCanvas(canvas);
            
            std::string compName = data->availableComponents[i];
            button->Subscribe(Button::ClickState::JustPressed, [data, compName]() {
                if (data->onComponentSelected) data->onComponentSelected(compName);
            });
        }
    }
    
    float yOffset = basePosition.y + yOffsetComponentsStart + data->scrollOffset;
    for (const auto& compData : data->components) {
        GameObject* compGO = scene->CreateGameObject();
        
        Image* compBg = UtilitUI::AddImage(compGO, componentBgTex,
            Vector2(basePosition.x - 300, yOffset), Vector2(400, 80),
            baseAnchorMin, baseAnchorMax,
            glm::vec4(0.25f, 0.25f, 0.3f, 1.0f));
        compBg->SetLayer(UtilitUI::Layers::InspectorButton);
        canvas->AddUIElement(compBg);
        
        Text* compName = UtilitUI::AddTextWithAnchors(compGO, font, compData.componentName,
            Vector2(basePosition.x - 280, yOffset + 25), Vector2(200, 25),
            baseAnchorMin, baseAnchorMax,
            glm::vec4(0.9f, 0.9f, 1.0f, 1.0f));
        compName->SetLayer(UtilitUI::Layers::InspectorText);
        canvas->AddUIElement(compName);
        
        GameObject* removeBtnGO = scene->CreateGameObject();
        
        Image* removeBg = UtilitUI::AddImage(removeBtnGO, buttonTex,
            Vector2(basePosition.x + 50, yOffset + 25), Vector2(60, 25),
            baseAnchorMin, baseAnchorMax,
            glm::vec4(0.7f, 0.2f, 0.2f, 1.0f));
        removeBg->SetLayer(UtilitUI::Layers::InspectorButton);
        canvas->AddUIElement(removeBg);
        
        Text* removeText = UtilitUI::AddTextWithAnchors(removeBtnGO, font, "X",
            Vector2(basePosition.x + 50, yOffset + 28), Vector2(60, 20),
            baseAnchorMin, baseAnchorMax,
            glm::vec4(1.0f));
        removeText->SetLayer(UtilitUI::Layers::InspectorText);
        canvas->AddUIElement(removeText);
        
        Button* removeButton = removeBtnGO->AddComponent<Button>();
        removeButton->SetUIElement(removeBg);
        removeButton->SetCanvas(canvas);
        removeButton->Subscribe(Button::ClickState::JustPressed, [data, compData]() {
            if (data->onRemoveComponent) data->onRemoveComponent(compData.componentName);
        });
        
        float paramY = yOffset - 10;
        for (const auto& [paramName, paramValue] : compData.params) {
            paramY -= ySpacingParams;
            
            Text* paramText = UtilitUI::AddTextWithAnchors(compGO, font, paramName + ":",
                Vector2(basePosition.x - 280, paramY), Vector2(120, 25),
                baseAnchorMin, baseAnchorMax,
                glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
            paramText->SetLayer(UtilitUI::Layers::InspectorText);
            canvas->AddUIElement(paramText);
            
            GameObject* valueBtnGO = scene->CreateGameObject();
            
            Image* valueBg = UtilitUI::AddImage(valueBtnGO, inputBgTex,
                Vector2(basePosition.x - 140, paramY), Vector2(200, 25),
                baseAnchorMin, baseAnchorMax,
                glm::vec4(0.15f, 0.15f, 0.2f, 1.0f));
            valueBg->SetLayer(UtilitUI::Layers::InspectorButton);
            canvas->AddUIElement(valueBg);
            
            Text* valueText = UtilitUI::AddTextWithAnchors(valueBtnGO, font, paramValue,
                Vector2(basePosition.x - 140, paramY + 3), Vector2(190, 20),
                baseAnchorMin, baseAnchorMax,
                glm::vec4(1.0f));
            valueText->SetLayer(UtilitUI::Layers::InspectorText);
            canvas->AddUIElement(valueText);
            
            Button* valueButton = valueBtnGO->AddComponent<Button>();
            valueButton->SetUIElement(valueBg);
            valueButton->SetCanvas(canvas);
        }
        
        yOffset -= ySpacingComponents;
    }
}

void InspectorUI::SetDrawData(InspectorDrawData* data) {
    currentData = data;
}*/