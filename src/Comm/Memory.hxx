// Copyright [2025] TaCAD

#ifndef COMM_OCCHANDLE_H_
#define COMM_OCCHANDLE_H_

#include <utility> // For std::forward()

#include <Standard_Handle.hxx>

// #define Handle(Class) opencascade::handle<Class>
#define Shared(Class) std::shared_ptr<Class>
#define Weaker(Class) std::weak_ptr<Class>

template<typename T, typename... Args>
Shared(T) MakeShared(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

// Constructs an object of type 'T' wrapped in an OpenCascade handle
// Note: Standard_Transient must be a base class of 'T'
template<typename T, typename... Args>
Handle(T) MakeHandle(Args&&... args)
{
    static_assert(std::is_base_of<Standard_Transient, T>::value,
                  "T must be derived from Standard_Transient");
    return new T(std::forward<Args>(args)...);
}

#endif  // COMM_OCCHANDLE_H_
