// Copyright [2025] TaCAD

#pragma once

#include <boost/signals2.hpp>

template<typename T>
using Signal = boost::signals2::signal<T>;

template<typename T>
using SignalConnection = boost::signals2::connection;
