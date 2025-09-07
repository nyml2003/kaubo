#pragma once

namespace kaubo {

void init_runtime_support();

void force_utf8();

void handle_mode_compile();

void handle_mode_interpret();

void handle_mode_interpret_bytecode();

}  // namespace kaubo