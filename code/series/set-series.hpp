//
// Created by Vadma on 05.12.2024.
//

#ifndef REGVALVE_SET_SERIES_HPP
#define REGVALVE_SET_SERIES_HPP

#include <vector>
#include <set>

template <class Series>
class SetSeries {
private:
    std::multiset <double> m_min_x, m_max_x, m_min_y, m_max_y;
    std::vector <Series> series;

public:
    SetSeries() = default;

    void push_back(Series&& value) {
        m_min_x.insert(value.min_x());
        m_max_x.insert(value.max_x());
        m_min_y.insert(value.min_y());
        m_max_y.insert(value.max_y());
        series.push_back(value);
    }

    void pop_back() {
        const auto& last = series.back();
        m_min_x.erase(m_min_x.find(last.min_x()));
        m_max_x.erase(m_max_x.find(last.max_x()));
        m_min_y.erase(m_min_y.find(last.min_y()));
        m_max_y.erase(m_max_y.find(last.max_y()));
        series.pop_back();
    }

    inline const Series& back() const {
        return series.back();
    }

    inline bool empty() const {
        return series.empty();
    }

    [[nodiscard]] double min_x() const {
        return *m_min_x.begin();
    }

    [[nodiscard]] double max_x() const {
        return *m_max_x.rbegin();
    }

    [[nodiscard]] double min_y() const {
        return *m_min_y.begin();
    }

    [[nodiscard]] double max_y() const {
        return *m_max_y.rbegin();
    }
};

#endif //REGVALVE_SET_SERIES_HPP
