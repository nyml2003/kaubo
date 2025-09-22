#pragma once
#include "Object/Runtime/PyCode.h"

namespace kaubo {
void interpret(const Object::PyCodePtr& code);

auto compile(std::string input) -> Object::PyCodePtr;
auto CreateInput() -> std::string;

}  // namespace kaubo