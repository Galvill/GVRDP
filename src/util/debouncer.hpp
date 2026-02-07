#pragma once

#include <chrono>
#include <functional>
#include <optional>

namespace gvrdp {

// Timer-based debouncer for resize events.
// Call trigger() when an event occurs. Poll poll() each frame.
// After the quiet period elapses, the callback fires once.
class Debouncer {
public:
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::milliseconds;
    using Callback = std::function<void()>;

    explicit Debouncer(Duration quiet_period, Callback callback);

    void trigger();
    void poll();
    void cancel();
    bool is_pending() const;

private:
    Duration quiet_period_;
    Callback callback_;
    std::optional<Clock::time_point> deadline_;
};

}  // namespace gvrdp
