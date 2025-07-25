#ifndef _PROJECT_PCH_H_
#define _PROJECT_PCH_H_

#ifdef _WIN32
#include <windows.h>
#include <wingdi.h>
#elif defined(__linux__)
#include <GL/glx.h>
#elif defined(__APPLE__)
#include <OpenGL/OpenGL.h>
#endif

#include <any>
#include <string>

#include <QDebug>
#include <QMessageBox>
#include <QObject>
#include <QSharedPointer>

// occt includes
#include <BRepPrimAPI_MakeBox.hxx>
#include <Message.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_Name.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TDF_Reference.hxx>

// enum-flags include
#include "enumFlags/flags.hpp"

// Project includes
#include "Comm/Signal.hxx"
#include "Comm/PropertyChanged.hxx"
#include "Comm/Color.hxx"
#include "Comm/Memory.hxx"
#include "Comm/Math.hxx"
#include "Comm/ResourceUtils.hxx"
#include "Comm/SketchSolve.hxx"
#include "Comm/OcctHelper.hxx"

// ∏®÷˙¿‡–Õ£∫≈–∂œ «∑Ò¥Ê‘⁄ Set(args...)
template<typename T, typename... Args>
auto TrySet(T* obj, const TDF_Label&, Args&&... args) -> decltype(T::Set(std::forward<Args>(args)...), void())
{
    T::Set(std::forward<Args>(args)...);
}

// »Ù…œ ˆ≤ª∆•≈‰£¨‘Ú≥¢ ‘ Set(label, args...)
template<typename T, typename... Args>
auto TrySet(T* obj, const TDF_Label& label, Args&&... args) -> decltype(T::Set(label, std::forward<Args>(args)...), void())
{
    T::Set(label, std::forward<Args>(args)...);
}


#define DEFINE_ENUM_FLAGS(FlagsType, EnumType)                                          \
    ALLOW_FLAGS_FOR_ENUM(EnumType);                                                     \
    using FlagsType = flags::flags<EnumType>;


#define DEFINE_ATTRIBUTE_ON_LABEL(attrType, attrName)                                   \
public: TDF_Label attrName##Label() const                                               \
{                                                                                       \
	return Label();                                                                     \
}                                                                                       \
public: Handle(attrType) attrName##Attribute() const                                    \
{                                                                                       \
	Handle(attrType) anAttr;                                                            \
	attrName##Label().FindAttribute(attrType::GetID(), anAttr);                         \
	return anAttr;                                                                      \
}


#define DEFINE_ATTRIBUTE_ON_CHILD(index, attrType, attrName)                            \
public: TDF_Label attrName##Label() const                                               \
{                                                                                       \
    return Label().FindChild(index);                                                    \
}                                                                                       \
public: Handle(attrType) attrName##Attribute() const                                    \
{                                                                                       \
    Handle(attrType) anAttr;                                                            \
    attrName##Label().FindAttribute(attrType::GetID(), anAttr);                         \
    return anAttr;                                                                      \
}                                                                                       \
public: template<typename... Args> void TrySet##attrName(Args&&... args) const          \
{                                                                                       \
    TrySet(attrName##Attribute().get(), attrName##Label(), std::forward<Args>(args)...);\
}                                                                                       \
public: void Init##attrName() const                                                     \
{                                                                                       \
    if(attrName##Attribute().IsNull())                                                  \
        attrName##Label().AddAttribute(new attrType);                                   \
}

#define DEFINE_REFERENCE_ON_LABEL(refType, refName)                                     \
public: TDF_Label refName##Label() const                                                \
{                                                                                       \
    Handle(TDF_Reference) ref;                                                          \
    if (Label().FindAttribute(TDF_Reference::GetID(), ref))                             \
        return ref->Get();                                                              \
	return TDF_Label();                                                                 \
}                                                                                       \
public: Handle(refType) refName##Attribute() const                                      \
{                                                                                       \
    return SketchFactory::Wrap<refType>(refName##Label());                              \
}

#endif // _PROJECT_PCH_H_
