#include <micro/debug/ParamManager.hpp>
#include <micro/utils/units.hpp>

#include <micro/test/utils.hpp>

using namespace micro;

TEST(ParamManager, all) {
    ParamManager params;

    bool b = false;
    int8_t i8 = 8;
    int16_t i16 = 16;
    int32_t i32 = 32;
    uint8_t u8 = 80;
    uint16_t u16 = 160;
    uint32_t u32 = 320;
    float f = 1.0f;
    meter_t m(10.0f);

    params.registerParam("b", b);
    params.registerParam("i8", i8);
    params.registerParam("i16", i16);
    params.registerParam("i32", i32);
    params.registerParam("u8", u8);
    params.registerParam("u16", u16);
    params.registerParam("u32", u32);
    params.registerParam("f", f);
    params.registerParam("m", m);

    const auto all = params.getAll();
    ASSERT_EQ(9, all.size());
    EXPECT_EQ(false, std::get<bool>(all.at("b")));
    EXPECT_EQ(8, std::get<int8_t>(all.at("i8")));
    EXPECT_EQ(16, std::get<int16_t>(all.at("i16")));
    EXPECT_EQ(32, std::get<int32_t>(all.at("i32")));
    EXPECT_EQ(80, std::get<uint8_t>(all.at("u8")));
    EXPECT_EQ(160, std::get<uint16_t>(all.at("u16")));
    EXPECT_EQ(320, std::get<uint32_t>(all.at("u32")));
    EXPECT_EQ(1.0f, std::get<float>(all.at("f")));
    EXPECT_EQ(10.0f, std::get<underlying_type_t<meter_t>>(all.at("m")));
}

TEST(ParamManager, value_changed) {
    ParamManager params;

    int32_t i32 = 1;
    params.registerParam("i32", i32);

    i32 = 2;

    const auto changed1 = params.sync();
    ASSERT_EQ(1, changed1.size());
    EXPECT_EQ(2, std::get<int32_t>(changed1.at("i32")));

    const auto changed2 = params.sync();
    ASSERT_EQ(0, changed2.size());

    const auto all = params.getAll();
    ASSERT_EQ(1, all.size());
    EXPECT_EQ(2, std::get<int32_t>(all.at("i32")));
}

TEST(ParamManager, new_values) {
    ParamManager params;

    int32_t i32 = 1;
    uint32_t u32 = 10;

    params.registerParam("i32", i32);
    params.registerParam("u32", u32);

    const auto changed = params.update({{"i32", static_cast<int32_t>(2)}});
    ASSERT_EQ(1, changed.size());
    EXPECT_EQ(2, std::get<int32_t>(changed.at("i32")));

    const auto all = params.getAll();
    ASSERT_EQ(2, all.size());
    EXPECT_EQ(2, std::get<int32_t>(all.at("i32")));
    EXPECT_EQ(10, std::get<uint32_t>(all.at("u32")));
}
