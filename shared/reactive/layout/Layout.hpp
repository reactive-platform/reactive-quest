#pragma once

#include <memory>
#include <vector>

#include "reactive/ReactiveComponent.hpp"
#include "reactive/layout/Props.hpp"
#include "reactive/yoga/YogaLayoutController.hpp"

namespace reactive {
template <class P>
concept HasLayoutProps = HasItemProps<P> && requires(P const& p) {
    p.flexController;
    p.children;
};

struct LayoutProps {
    REACTIVE_LAYOUT_PROPS;
};

class Layout : public ReactiveComponent {
public:
    using Props = LayoutProps;

    Layout() = default;
    explicit Layout(Props const& props) : Layout(props, 0) {}
    ~Layout() override;

    YogaLayoutController* GetLayoutController() const { return _controller.get(); }
    YogaLayoutController& FlexController();

    void AddChild(IReactiveComponent* item);
    void RemoveChild(IReactiveComponent* item);
    void ClearChildren();

    void DetachChild(HostState* host);

    template <class Self>
    Self* AsFlexGroup(this Self& self, FlexControllerProps const& props = {});

protected:
    template <HasLayoutProps P>
    Layout(P const& props, int) : ReactiveComponent(props) {
        ApplyLayoutProps(props.flexController, props.children);
    }

    void ApplyLayoutProps(FlexControllerProps const& controller, std::vector<IReactiveComponent*> const& children);

    void OnInitialize() override;
    void OnRecalculateLayoutSelf() override;
    void OnLayoutApply() override;

private:
    std::vector<IReactiveComponent*> _initialChildren;
    std::vector<IReactiveComponent*> _children;

    std::unique_ptr<YogaLayoutController> _controller;

    void AttachController();
};
}
