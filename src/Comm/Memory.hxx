// Copyright [2025] TaCAD

#ifndef COMM_OCCHANDLE_H_
#define COMM_OCCHANDLE_H_

#include <memory>
#include <utility>

#include <Standard_Handle.hxx>

// Type aliases for memory management
// Use Handle for OpenCascade objects (T must derive from Standard_Transient)
// Use Shared and Weaker for C++ standard smart pointers
// Use Unique for exclusive ownership with std::unique_ptr
template<typename T>
using Handle = opencascade::handle<T>;

template<typename T>
using Shared = std::shared_ptr<T>;

template<typename T>
using Weaker = std::weak_ptr<T>;

template<typename T>
using Unique = std::unique_ptr<T>;

// Constructs an object of type 'T' wrapped in an OpenCascade handle
// Note: Standard_Transient must be a base class of 'T'
// @throws std::bad_alloc if memory allocation fails
template<typename T, typename... Args>
Handle<T> MakeHandle(Args&&... args) {
    static_assert(std::is_base_of<Standard_Transient, T>::value,
        "T must be derived from Standard_Transient");
    return new T(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
Shared<T> MakeShared(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

// Constructs an object of type 'T' wrapped in a std::unique_ptr
// @throws std::bad_alloc if memory allocation fails
template<typename T, typename... Args>
Unique<T> MakeUnique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

#endif // COMM_OCCHANDLE_H_