#pragma once

#include "EditorUI.h"
#include "EditorLogic.h"
#include <variant>

template<typename... TData, template<typename> typename TUI>
class EditorBase {
    static_assert(std::is_base_of_v<EditorUI<std::variant<TData...>>, 
                                    TUI<std::variant<TData...>>>,
                  "TUI<DataTypes> must be derived from EditorUI<DataTypes>");

protected:
    using DataTypes = std::variant<TData...>;
    
    TUI<DataTypes>* ui{nullptr};
    EditorLogic<TUI, DataTypes>* logic{nullptr};

public:
    EditorBase() {
        ui = CreateUI();
        logic = CreateLogic();
        if (logic && ui) {
            logic->SetUI(ui);
        }
    }
    
    virtual ~EditorBase() {
        delete ui;
        delete logic;
    }
    
    virtual TUI<DataTypes>* CreateUI() = 0;
    virtual EditorLogic<TUI, DataTypes>* CreateLogic() = 0;
    
    TUI<DataTypes>* GetUI() { return ui; }
    EditorLogic<TUI, DataTypes>* GetLogic() { return logic; }
    
    void Start() { 
        if (logic) logic->Start(); 
        if (ui) ui->Start(); 
    }
    
    void Update() { 
        if (logic) logic->Update(); 
        if (ui) ui->Update(); 
    }
    
    void Destroy() { 
        if (logic) logic->Destroy(); 
        if (ui) ui->Destroy(); 
    }
};