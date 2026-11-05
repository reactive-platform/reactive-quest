#pragma once

namespace reactive {
class IStateBase;

// States flush first, so a derived state is evaluated once however many of its dependencies changed, then the layout they caused.
struct ReactiveScheduler {
    static void ScheduleState(IStateBase* state);

    static void Run();
};
}
