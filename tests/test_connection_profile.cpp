#include "config/connection_profile.hpp"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace gvrdp;

TEST(ConnectionProfile, DefaultValues) {
    ConnectionProfile p;
    EXPECT_EQ(p.port, 3389);
    EXPECT_EQ(p.width, 1920u);
    EXPECT_EQ(p.height, 1080u);
    EXPECT_EQ(p.color_depth, 32u);
    EXPECT_TRUE(p.dynamic_resolution);
    EXPECT_TRUE(p.enable_clipboard);
    EXPECT_TRUE(p.enable_audio);
    EXPECT_FALSE(p.enable_drive_redirect);
    EXPECT_FALSE(p.fullscreen);
}

TEST(ConnectionProfile, JsonRoundTrip) {
    ConnectionProfile original;
    original.name = "TestServer";
    original.hostname = "192.168.1.100";
    original.port = 3390;
    original.username = "admin";
    original.domain = "WORKGROUP";
    original.width = 2560;
    original.height = 1440;
    original.dynamic_resolution = false;
    original.enable_clipboard = false;

    nlohmann::json j = original;
    auto restored = j.get<ConnectionProfile>();

    EXPECT_EQ(restored.name, "TestServer");
    EXPECT_EQ(restored.hostname, "192.168.1.100");
    EXPECT_EQ(restored.port, 3390);
    EXPECT_EQ(restored.username, "admin");
    EXPECT_EQ(restored.domain, "WORKGROUP");
    EXPECT_EQ(restored.width, 2560u);
    EXPECT_EQ(restored.height, 1440u);
    EXPECT_FALSE(restored.dynamic_resolution);
    EXPECT_FALSE(restored.enable_clipboard);
}

TEST(ConnectionProfile, PartialJsonDeserialization) {
    // Only specify some fields; others should get defaults
    nlohmann::json j = {{"hostname", "myserver.example.com"}, {"port", 13389}};

    auto profile = j.get<ConnectionProfile>();
    EXPECT_EQ(profile.hostname, "myserver.example.com");
    EXPECT_EQ(profile.port, 13389);
    EXPECT_EQ(profile.width, 1920u);  // default
    EXPECT_TRUE(profile.dynamic_resolution);  // default
    EXPECT_TRUE(profile.enable_clipboard);    // default
}

TEST(ConnectionProfile, PasswordNotSerialized) {
    ConnectionProfile p;
    p.password = "secret123";
    nlohmann::json j = p;

    // Password is intentionally excluded from the JSON macro for security.
    // It should NOT appear in serialized output.
    EXPECT_FALSE(j.contains("password"));

    // Deserialized profile should have empty password
    auto restored = j.get<ConnectionProfile>();
    EXPECT_TRUE(restored.password.empty());
}
