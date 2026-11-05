#pragma once

#include <optional>

#include "reactive/state/State.hpp"

namespace reactive {
template <class T>
class CellContext final : public StateBase<CellContext<T>> {
public:
    CellContext const& Value() const override { return *this; }

    T const& Item() const { return *_item; }
    bool     HasItem() const { return _item.has_value(); }
    int      Index() const { return _index; }
    int      TotalCells() const { return _totalCells; }

    bool Selected() const { return _selected; }
    void SetSelected(bool v) {
        if (_selected == v) return;
        _selected = v;
        this->NotifyValueInvalidated();
    }

    void Init(T const& item, int index, int totalCells) {
        bool updated = _index != index || _totalCells != totalCells;
        // C++ models have no equality, so only cheaply comparable types are compared.
        if constexpr (detail::kStateSkipsEqual<T>) {
            updated = updated || !_item || !(*_item == item);
        } else {
            updated = true;
        }
        if (!updated) return;

        _item       = item;
        _index      = index;
        _totalCells = totalCells;
        this->NotifyValueInvalidated();
    }

private:
    std::optional<T> _item;
    int              _index{-1};
    int              _totalCells{0};
    bool             _selected{false};
};
}
