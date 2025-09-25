#pragma once
#include "Object/Runtime/PyCode.h"

namespace kaubo {
void interpret(const Object::PyCodePtr& code);

auto compile(const std::string& source) -> Object::PyCodePtr;
auto CreateInput() -> std::string;

}  // namespace kaubo