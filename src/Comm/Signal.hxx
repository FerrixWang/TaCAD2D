// Copyright [2025] TaCAD

#ifndef _Signal_Header_File_
#define _Signal_Header_File_

#include <boost/signals2.hpp>

template<typename T>
using Signal = boost::signals2::signal<T>;

template<typename T>
using SignalConnection = boost::signals2::connection;

#endif  // _Signal_Header_File_
