

// Copyright [2025] TaCAD

#ifndef OCCT_OCCTHELPER_OcctHelper_H_
#define OCCT_OCCTHELPER_OcctHelper_H_

#include <string>

#include <TCollection_ExtendedString.hxx>

class OcctHelper
{
public:
    // 转换 TCollection_ExtendedString 到 std::string
    static std::string ExtendedStringToStdString(const TCollection_ExtendedString& extStr)
    {
        // 使用 WideChar 转换为 UTF-8 std::string
        std::wstring wideStr(extStr.ToWideString());
        return std::string(wideStr.begin(), wideStr.end());
    }
};

#endif  // OCCT_OCCTHELPER_OcctHelper_H_
