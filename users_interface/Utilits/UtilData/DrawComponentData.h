#pragma once

struct DrawComponentData{
    std::string componentName;
    std::unordered_map<std::string, std::string> nameToParams;

    DrawComponentData() = default;
    DrawComponentData(std::string componentName)
    : componentName(componentName)  {}
};