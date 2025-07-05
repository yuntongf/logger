#pragma once

#include "formatter.h"

class EffectiveFormatter : public IFormatter {
public:
    void serialize(const LogMsg& msg, void* dest, std::size_t& output_size) const override;
};