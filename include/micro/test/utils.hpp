#pragma once

#include <micro/math/unit_utils.hpp>

#include "gtest/gtest.h"

#define EXPECT_EQ_UNIT(a, b)        EXPECT_EQ(a.get(), static_cast<decltype(a)>(b).get())
#define EXPECT_NEAR_UNIT(a, b, eps) EXPECT_NEAR(a.get(), static_cast<decltype(a)>(b).get(), static_cast<decltype(a)>(eps).get())
