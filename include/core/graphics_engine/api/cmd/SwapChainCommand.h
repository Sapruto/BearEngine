#pragma once

#include "RHI/Commands/BaseCommand.h"

enum class IntervalType {
    FIFO,
    IMMEDIATE,
    FIFO_RELAXED,
    MAILBOX
};

struct SwapChainCommand : public BaseCommand {
    IntervalType intervalType;
    uint32_t swapChainSize;
    bool isResizeRequested;
    uint32_t newWidth;
    uint32_t newHeight;

    SwapChainCommand() 
        : intervalType(IntervalType::FIFO)
        , swapChainSize(2)
        , isResizeRequested(false)
        , newWidth(0)
        , newHeight(0) {}
};