#pragma once

#include <cstdio>

//* config

#define SR_LA_LOG_Allocator_Do 0
#define SR_LA_LOG_Allocator_Info_Do 1
#define SR_LA_LOG_Arena_Do 0

//* macros

#if SR_LA_LOG_Allocator_Do
constexpr auto SR_LA_LA_LOG = printf;
#else
constexpr auto SR_LA_LA_LOG = [](...) {
};
#endif

#if SR_LA_LOG_Arena_Do
constexpr auto SR_LA_A_LOG = printf;
#else
constexpr auto SR_LA_A_LOG = [](...) {
};
#endif

#if SR_LA_LOG_Allocator_Info_Do
constexpr auto SR_LA_LA_I_LOG = printf;
#else
constexpr auto SR_LA_LA_I_LOG = [](...) {
};
#endif