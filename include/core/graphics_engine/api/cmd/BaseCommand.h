#pragma once

struct BaseCommand {
protected:
    CommandType type;
    unsigned int layer{0};

public:
    BaseCommand() = default;

    CommandType GetCommandType() { return type; }
    int GetLayer() { return layer; }

    void SetLayer(unsigned int newLayer) { layer = newLayer; }
};