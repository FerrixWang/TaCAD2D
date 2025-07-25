
// Copyright [2025] TaCAD

#ifndef _OcctHelper_Header_File_
#define _OcctHelper_Header_File_

#include <string>

#include <TCollection_ExtendedString.hxx>

class OcctHelper
{
public:
    static std::string ExtendedStringToStdString(const TCollection_ExtendedString& extStr)
    {
        std::wstring wideStr(extStr.ToWideString());
        return std::string(wideStr.begin(), wideStr.end());
    }
};

#endif  // _OcctHelper_Header_File_
