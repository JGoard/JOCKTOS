/**************************************************************************//**
 * @file     cmsis_compiler.h
 * @brief    CMSIS compiler common header file
 * @date     
 ******************************************************************************/

#ifndef __CMSIS_COMPILER_H__
#define __CMSIS_COMPILER_H__

#if defined ( __GNUC__ )
#  include "CMSIS5/cmsis_gcc.h"
#else
#  error Unsupported compiler, currently only GCC is supported.
#endif

#endif/*__CMSIS_COMPLILER_H__*/
