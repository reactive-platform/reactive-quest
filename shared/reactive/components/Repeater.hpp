#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "reactive/ReactiveComponent.hpp"
#include "reactive/components/CellContext.hpp"
#include "reactive/layout/Layout.hpp"
#include "reactive/layout/Props.hpp"
#include "reactive/yoga/YogaLayoutController.hpp"

namespace reactive {
template <class TItem, class TCell = IReactiveComponent>
struct RepeaterProps {
    REACTIVE_ITEM_PROPS;
    FlexControllerProps flexController{};
    std::function<TCell*(std::shared_ptr<CellContext<TItem>>)> constructCell;
    REACTIVE_PROP(std::vector<TItem>, items, sItems);
};

template <class TItem, class TCell = IReactiveComponent>
class Repeater : public ReactiveComponent {
public:
    using Props       = RepeaterProps<TItem, TCell>;
    using Context     = CellContext<TItem>;
    using CellFactory = std::function<TCell*(std::shared_ptr<Context>)>;

    explicit Repeater(Props const& props)
        : ReactiveComponent(props), _container(new Layout()), _constructCell(props.constructCell) {
        _container->FlexController().SetFlexDirection(FlexDirection::Column);
        _container->FlexController().Apply(props.flexController);
        BindProp(props.items, props.sItems, [this](std::vector<TItem> const& v) { SetItems(v); });
    }

    void SetItems(std::vector<TItem> items) {
        _items = std::move(items);
        RefreshCells();
    }

    YogaLayoutController& FlexController() { return _container->FlexController(); }

protected:
    UnityEngine::GameObject* Construct() override {
        return _container->Use(nullptr);
    }

private:
    Layout*                                                  _container;
    CellFactory                                              _constructCell;
    std::vector<TItem>                                       _items;
    std::vector<std::pair<std::shared_ptr<Context>, TCell*>> _cells;

    void RefreshCells() {
        const int total = static_cast<int>(_items.size());
        for (int i = 0; i < total; ++i) {
            auto const& item = _items[i];
            if (static_cast<size_t>(i) >= _cells.size()) {
                // Init the context before constructing so the cell's bindings read the item right away.
                auto context = std::make_shared<Context>();
                context->Init(item, i, total);
                TCell* cell = _constructCell(context);
                _cells.emplace_back(context, cell);
                _container->AddChild(cell);
            } else {
                auto& [context, cell] = _cells[i];
                context->Init(item, i, total);
                cell->SetEnabled(true);
            }
        }
        for (size_t i = _items.size(); i < _cells.size(); ++i) {
            _cells[i].second->SetEnabled(false);
        }
    }
};
}
