#include "util/debouncer.hpp"

namespace gvrdp {

Debouncer::Debouncer(Duration quiet_period, Callback callback)
    : quiet_period_(quiet_period), callback_(std::move(callback)) {}

void Debouncer::trigger() {
    deadline_ = Clock::now() + quiet_period_;
}

void Debouncer::poll() {
    if (deadline_ && Clock::now() >= *deadline_) {
        deadline_.reset();
        if (callback_) {
            callback_();
        }
    }
}

void Debouncer::cancel() {
    deadline_.reset();
}

bool Debouncer::is_pending() const {
    return deadline_.has_value();
}

}  // namespace gvrdp
