#pragma once

#include <functional>
#include <memory>
#include <optional>

#include "reactive/ReactiveComponent.hpp"
#include "reactive/components/ScrollContext.hpp"

namespace reactive::beatsaber {
enum class ScrollbarScrollMode {
    Page,
    Line,
    Fixed,
};

struct ScrollbarProps {
    REACTIVE_ITEM_PROPS;
    std::shared_ptr<::reactive::components::ScrollContext> scrollContext;
    std::optional<ScrollbarScrollMode> scrollMode;
    std::optional<float> fixedScrollSize;
    std::optional<bool> hideIfNothingToScroll;
};

// Scrollbar for ReactiveComponent lists.
class Scrollbar : public ReactiveComponent {
public:
    using Props = ScrollbarProps;

    explicit Scrollbar(Props const& props);

protected:
    UnityEngine::GameObject* Construct() override;
    void OnInitialize() override;
    void OnRectDimensionsChanged() override { RefreshHandle(); }

private:
    std::shared_ptr<::reactive::components::ScrollContext> _scrollContext;
    ScrollbarScrollMode _scrollMode;
    float               _fixedScrollSize;
    bool                _hideIfNothingToScroll;

    UnityEngine::RectTransform* _handleContainerRect{nullptr};
    UnityEngine::RectTransform* _handleRect{nullptr};

    IReactiveComponent* CreateButton(bool topButton);
    void Scroll(bool down);
    void RefreshHandle();
};
}
