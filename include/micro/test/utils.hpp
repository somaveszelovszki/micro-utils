#pragma once

#include <micro/math/unit_utils.hpp>

#include "gtest/gtest.h"

#define EXPECT_NEAR_DEFAULT(a, b)      EXPECT_NEAR((a), (b), decltype(a)(0.0001f))
#define EXPECT_EQ_UNIT(a, b)           EXPECT_EQ((a).get(), static_cast<decltype(a)>(b).get())
#define EXPECT_NEAR_UNIT(a, b, eps)    EXPECT_NEAR((a).get(), static_cast<decltype(a)>(b).get(), static_cast<decltype(a)>(eps).get())
#define EXPECT_NEAR_UNIT_DEFAULT(a, b) EXPECT_NEAR_UNIT((a), (b), decltype(a)(0.0001f))

#define EXPECT_EQ_PAIR(a, b)         \
    EXPECT_EQ((a).first, (b.first)); \
    EXPECT_EQ((a).second, (b.second))
