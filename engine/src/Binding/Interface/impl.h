#pragma once
#include "Object/Runtime/PyCode.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-user-provided-dtor"
#pragma clang diagnostic ignored "-Wdeprecated-copy-with-dtor"

#include <antlr4-runtime.h>

#pragma clang diagnostic pop
namespace kaubo {
void interpret(const Object::PyCodePtr& code);

using InputStreamPtr = std::unique_ptr<antlr4::ANTLRInputStream>;
Object::PyCodePtr compile(InputStreamPtr input_stream);
InputStreamPtr CreateANTLRInputStream();

}  // namespace kaubo