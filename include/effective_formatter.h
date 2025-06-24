#pragma once

#include "formatter.h"

class EffectiveFormatter : public IFormatter {
public:
    void serialize(const LogMsg& msg, void* dest) const override;
};