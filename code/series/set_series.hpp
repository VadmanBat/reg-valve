#pragma once

#include <set>
#include <vector>

template <class SeriesT>
class SetSeries {
    std::multiset<double> m_min_x, m_max_x, m_min_y, m_max_y;
    std::vector<SeriesT> series;

public:
    SetSeries() = default;

    void push_back(SeriesT value) {
        m_min_x.insert(value.min_x());
        m_max_x.insert(value.max_x());
        m_min_y.insert(value.min_y());
        m_max_y.insert(value.max_y());
        series.push_back(std::move(value));
    }

    void pop_back() {
        if (series.empty())
            return;
        const auto& last = series.back();
        m_min_x.erase(m_min_x.find(last.min_x()));
        m_max_x.erase(m_max_x.find(last.max_x()));
        m_min_y.erase(m_min_y.find(last.min_y()));
        m_max_y.erase(m_max_y.find(last.max_y()));
        series.pop_back();
    }

    void clear() {
        series.clear();
        m_min_x.clear();
        m_max_x.clear();
        m_min_y.clear();
        m_max_y.clear();
    }

    [[nodiscard]] const SeriesT& back() const { return series.back(); }
    [[nodiscard]] bool empty() const { return series.empty(); }
    [[nodiscard]] std::size_t size() const { return series.size(); }

    [[nodiscard]] double min_x() const { return m_min_x.empty() ? 0 : *m_min_x.begin(); }
    [[nodiscard]] double max_x() const { return m_max_x.empty() ? 1 : *m_max_x.rbegin(); }
    [[nodiscard]] double min_y() const { return m_min_y.empty() ? 0 : *m_min_y.begin(); }
    [[nodiscard]] double max_y() const { return m_max_y.empty() ? 1 : *m_max_y.rbegin(); }
};
