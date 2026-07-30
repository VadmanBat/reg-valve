#pragma once

#include "code/series/axis-bounds.hpp"

#include <set>
#include <vector>

/// LIFO multiset of curve extents → global min/max across stacked series.
class BoundsSet {
    std::vector<AxisBounds> stack_;
    std::multiset<double> min_x_;
    std::multiset<double> max_x_;
    std::multiset<double> min_y_;
    std::multiset<double> max_y_;

public:
    void push_back(AxisBounds b) {
        min_x_.insert(b.min_x);
        max_x_.insert(b.max_x);
        min_y_.insert(b.min_y);
        max_y_.insert(b.max_y);
        stack_.push_back(b);
    }

    void pop_back() {
        if (stack_.empty())
            return;
        const AxisBounds& last = stack_.back();
        min_x_.erase(min_x_.find(last.min_x));
        max_x_.erase(max_x_.find(last.max_x));
        min_y_.erase(min_y_.find(last.min_y));
        max_y_.erase(max_y_.find(last.max_y));
        stack_.pop_back();
    }

    void clear() {
        stack_.clear();
        min_x_.clear();
        max_x_.clear();
        min_y_.clear();
        max_y_.clear();
    }

    [[nodiscard]] bool empty() const noexcept { return stack_.empty(); }

    [[nodiscard]] double min_x() const noexcept { return min_x_.empty() ? 0.0 : *min_x_.begin(); }
    [[nodiscard]] double max_x() const noexcept { return max_x_.empty() ? 1.0 : *max_x_.rbegin(); }
    [[nodiscard]] double min_y() const noexcept { return min_y_.empty() ? 0.0 : *min_y_.begin(); }
    [[nodiscard]] double max_y() const noexcept { return max_y_.empty() ? 1.0 : *max_y_.rbegin(); }
};
