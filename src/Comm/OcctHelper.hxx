
// Copyright [2025] TaCAD
#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include <TCollection_ExtendedString.hxx>

class OcctHelper
{
public:
    static std::string ExtendedStringToStdString(const TCollection_ExtendedString& extStr)
    {
#ifdef _WIN32
       // Windows: ToWideString 返回 wchar_t*
        return std::filesystem::path(extStr.ToWideString()).string();
#else
       // macOS/Linux: ToExtString 返回 unsigned short* (UTF-16)
        const char16_t* u16 = reinterpret_cast<const char16_t*>(extStr.ToExtString());
        std::u16string u16str(u16);  // 构造 u16string
        return std::filesystem::path(u16str).string();
#endif
    }

};
