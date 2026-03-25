#pragma once
#include <string>

std::u32string UTF8ToUTF32(const std::string& utf8);
std::string    UTF32ToUTF8(const std::u32string& input);
std::u32string UTF16ToUTF32(const std::u16string& input);
