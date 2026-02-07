#include "input/keyboard_map.hpp"

#include <gtest/gtest.h>

using namespace gvrdp;

TEST(KeyboardMap, LetterKeys) {
    auto a = sdl_scancode_to_rdp(SDL_SCANCODE_A);
    EXPECT_EQ(a.code, 0x1E);
    EXPECT_FALSE(a.extended);

    auto z = sdl_scancode_to_rdp(SDL_SCANCODE_Z);
    EXPECT_EQ(z.code, 0x2C);
    EXPECT_FALSE(z.extended);
}

TEST(KeyboardMap, NumberKeys) {
    auto one = sdl_scancode_to_rdp(SDL_SCANCODE_1);
    EXPECT_EQ(one.code, 0x02);
    EXPECT_FALSE(one.extended);

    auto zero = sdl_scancode_to_rdp(SDL_SCANCODE_0);
    EXPECT_EQ(zero.code, 0x0B);
    EXPECT_FALSE(zero.extended);
}

TEST(KeyboardMap, FunctionKeys) {
    auto f1 = sdl_scancode_to_rdp(SDL_SCANCODE_F1);
    EXPECT_EQ(f1.code, 0x3B);
    EXPECT_FALSE(f1.extended);

    auto f12 = sdl_scancode_to_rdp(SDL_SCANCODE_F12);
    EXPECT_EQ(f12.code, 0x58);
    EXPECT_FALSE(f12.extended);
}

TEST(KeyboardMap, ExtendedKeys) {
    auto rctrl = sdl_scancode_to_rdp(SDL_SCANCODE_RCTRL);
    EXPECT_EQ(rctrl.code, 0x1D);
    EXPECT_TRUE(rctrl.extended);

    auto insert = sdl_scancode_to_rdp(SDL_SCANCODE_INSERT);
    EXPECT_EQ(insert.code, 0x52);
    EXPECT_TRUE(insert.extended);

    auto up = sdl_scancode_to_rdp(SDL_SCANCODE_UP);
    EXPECT_EQ(up.code, 0x48);
    EXPECT_TRUE(up.extended);
}

TEST(KeyboardMap, SpecialKeys) {
    auto esc = sdl_scancode_to_rdp(SDL_SCANCODE_ESCAPE);
    EXPECT_EQ(esc.code, 0x01);

    auto enter = sdl_scancode_to_rdp(SDL_SCANCODE_RETURN);
    EXPECT_EQ(enter.code, 0x1C);
    EXPECT_FALSE(enter.extended);

    auto space = sdl_scancode_to_rdp(SDL_SCANCODE_SPACE);
    EXPECT_EQ(space.code, 0x39);
}

TEST(KeyboardMap, UnmappedReturnsZero) {
    // SDL_SCANCODE_UNKNOWN should not be mapped
    auto unknown = sdl_scancode_to_rdp(SDL_SCANCODE_UNKNOWN);
    EXPECT_EQ(unknown.code, 0);
    EXPECT_FALSE(unknown.extended);
}
