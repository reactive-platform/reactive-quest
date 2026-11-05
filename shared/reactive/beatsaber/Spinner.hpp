#pragma once

#include "reactive/ReactiveComponent.hpp"
#include "reactive/beatsaber/Image.hpp"

namespace reactive::beatsaber {
struct SpinnerProps {
    REACTIVE_ITEM_PROPS;
    ImageProps image{};
    std::optional<float> rotationSpeed;
};

class Spinner : public ReactiveComponent {
public:
    using Props = SpinnerProps;

    Spinner() : _image(new Image()) { _image->SetPreserveAspect(true); }
    explicit Spinner(Props const& props) : ReactiveComponent(props), _image(new Image()) {
        _image->SetPreserveAspect(true);
        _image->ApplyImageProps(props.image);
        if (props.rotationSpeed) RotationSpeed = *props.rotationSpeed;
    }

    float RotationSpeed{50.f};

    Image* GetImage() const { return _image; }

protected:
    UnityEngine::GameObject* Construct() override;
    void OnUpdate() override;

private:
    Image* _image{nullptr};
};
}
