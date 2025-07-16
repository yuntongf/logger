#pragma once

#include "formatter.h"

class EffectiveFormatter : public IFormatter {
public:
    void serialize(const LogMsg& msg, std::vector<uint8_t>& data) const override;
};