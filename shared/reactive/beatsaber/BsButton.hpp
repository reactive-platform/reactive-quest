#pragma once

#include <functional>
#include <memory>
#include <optional>

#include <UnityEngine/Color.hpp>

#include "reactive/ReactiveComponent.hpp"
#include "reactive/beatsaber/BeatSaberStyle.hpp"
#include "reactive/components/GraphicState.hpp"
#include "reactive/layout/Props.hpp"

#define REACTIVE_BS_BUTTON_PROPS                       \
    std::function<void()> onClick{};                   \
    REACTIVE_PROP(bool, interactable, sInteractable);  \
    std::optional<bool> showUnderline{};               \
    std::optional<float> skew{};                       \
    std::optional<::reactive::beatsaber::BsButtonColors> colors {}

namespace reactive::beatsaber {
using ConstructButtonContent =
    std::function<IReactiveComponent*(std::shared_ptr<IState<UnityEngine::Color>> color, std::shared_ptr<IState<float>> skew)>;

struct BsButtonProps {
    REACTIVE_ITEM_PROPS;
    REACTIVE_BS_BUTTON_PROPS;
    ConstructButtonContent constructContent;
};

// A Beat Saber styled button without any content.
class BsButton : public ReactiveComponent {
public:
    using Props = BsButtonProps;

    explicit BsButton(Props const& props);

    void SetShowUnderline(bool v) { _underlineEnabled->SetValue(v); }
    void SetSkew(float v) { _skew->SetValue(v); }
    void SetInteractable(bool v) { ::reactive::SetInteractable(*_graphicState, v); }
    void SetColors(BsButtonColors colors) { _colors->SetValue(std::move(colors)); }

    std::function<void()> OnClick;

protected:
    UnityEngine::GameObject* Construct() override;

private:
    ConstructButtonContent _constructContent;

    std::shared_ptr<State<bool>>           _underlineEnabled;
    std::shared_ptr<State<float>>          _skew;
    std::shared_ptr<State<GraphicState>>   _graphicState;
    std::shared_ptr<State<BsButtonColors>> _colors;
};
}
