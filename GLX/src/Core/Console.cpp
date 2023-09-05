#include "Console.h"

ImGuiTextBuffer Console::Buf;
ImGuiTextFilter Console::Filter;
ImVector<int> Console::LineOffsets;
bool Console::ScrollToBottom = false;