#include "util/debouncer.hpp"

#include <gtest/gtest.h>

#include <thread>

using namespace gvrdp;

TEST(Debouncer, DoesNotFireImmediately) {
    bool fired = false;
    Debouncer d(std::chrono::milliseconds(50), [&]() { fired = true; });

    d.trigger();
    d.poll();  // Should not fire yet
    EXPECT_FALSE(fired);
}

TEST(Debouncer, FiresAfterQuietPeriod) {
    bool fired = false;
    Debouncer d(std::chrono::milliseconds(20), [&]() { fired = true; });

    d.trigger();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    d.poll();
    EXPECT_TRUE(fired);
}

TEST(Debouncer, RetriggersResetDeadline) {
    int count = 0;
    Debouncer d(std::chrono::milliseconds(50), [&]() { count++; });

    d.trigger();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    d.trigger();  // Re-trigger, reset deadline
    d.poll();
    EXPECT_EQ(count, 0);  // Should not have fired yet

    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    d.poll();
    EXPECT_EQ(count, 1);  // Now it fires
}

TEST(Debouncer, CancelPreventsFireing) {
    bool fired = false;
    Debouncer d(std::chrono::milliseconds(20), [&]() { fired = true; });

    d.trigger();
    d.cancel();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    d.poll();
    EXPECT_FALSE(fired);
}

TEST(Debouncer, IsPending) {
    Debouncer d(std::chrono::milliseconds(100), []() {});

    EXPECT_FALSE(d.is_pending());
    d.trigger();
    EXPECT_TRUE(d.is_pending());
    d.cancel();
    EXPECT_FALSE(d.is_pending());
}
