#pragma once

#include "EditorUI.h"

template<template<typename> typename TUI, typename... TData>
class EditorLogic {
    static_assert(std::is_base_of_v<EditorUI<TData>, TUI<TData>>, 
                  "TUI<TData> must be derived from EditorUI<TData>");

protected:
    using DataTypes = std::variant<TData...>;

    TUI<DataTypes>* ui;

public:
    EditorLogic() = default;
    virtual ~EditorLogic() = default;

    void SetUI(TUI<TData>* ui) { this->ui = ui; }

    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void Destroy() = 0;
};