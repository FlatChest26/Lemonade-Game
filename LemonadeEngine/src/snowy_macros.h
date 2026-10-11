#pragma once

#ifndef _SNOWY_MACROS_H_
#define _SNOWY_MACROS_H_

#include <iostream>
#include <cassert>

#ifdef _DEBUG
#define IF_DEBUG(...) __VA_ARGS__
#else
#define IF_DEBUG(...)
#endif // _DEBUG

#define COUT(...) std::cout << __VA_ARGS__ << std::endl
#define CERR(...) std::cerr << "[ERROR] " << __FILE__ << ":" << __LINE__ << " (in " << __func__ << ") -> " << __VA_ARGS__ << std::endl

#define THROW_RUNTIME_ERR throw std::runtime_error

#define VALIDATE_OR_RET(x, ...) if (!(x)) { CERR(__VA_ARGS__); return false;}

#define ASSERT(cond, mesg) assert(cond && mesg)
#define DEBUG_ASSERT(cond, mesg) IF_DEBUG( ASSERT(cond, mesg) )
#define SNOWY_ERROR(mesg) _STL_REPORT_ERROR(mesg)

#endif // !_SNOWY_MACROS_H_
