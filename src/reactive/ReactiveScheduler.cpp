#include "reactive/ReactiveScheduler.hpp"

#include <algorithm>
#include <deque>
#include <exception>
#include <memory>
#include <vector>

#include "main.hpp"
#include "reactive/HostState.hpp"
#include "reactive/LayoutQueue.hpp"
#include "reactive/state/State.hpp"

namespace reactive {
int      StateInvalidation::Depth = 0;
uint64_t StateInvalidation::Walk  = 0;

namespace {
    constexpr int kMaxPasses  = 100;
    constexpr int kMaxFlushes = 100000;

    std::deque<std::weak_ptr<IStateBase>> g_stateQueue;
    std::vector<HostState*>               g_layoutQueue;

    bool g_stateRunning  = false;
    bool g_layoutRunning = false;
    bool g_running       = false;

    void RunStates() {
        if (g_stateRunning) return;
        g_stateRunning = true;
        for (int i = 0; !g_stateQueue.empty(); ++i) {
            if (i == kMaxFlushes) {
                ReactiveLogger.error("Reactive: states kept changing each other for {} flushes, "
                                     "the rest is left for the next run", kMaxFlushes);
                break;
            }
            auto state = g_stateQueue.front().lock();
            g_stateQueue.pop_front();
            if (!state) continue;
            try {
                state->Flush();
            } catch (std::exception const& e) {
                ReactiveLogger.error("Reactive: state callback threw: {}", e.what());
            } catch (...) {
                ReactiveLogger.error("Reactive: state callback threw an unknown exception");
            }
        }
        g_stateRunning = false;
    }
}

void ReactiveScheduler::ScheduleState(IStateBase* state) {
    g_stateQueue.push_back(state->weak_from_this());
}

void ReactiveScheduler::Run() {
    if (g_running || g_stateRunning || g_layoutRunning) return;
    g_running = true;
    for (int pass = 0; !g_stateQueue.empty() || layoutqueue::HasWork(); ++pass) {
        if (pass == kMaxPasses) {
            ReactiveLogger.error("Reactive: states and layout kept scheduling each other for {} passes, "
                                 "the rest is left for the next frame", kMaxPasses);
            break;
        }
        RunStates();
        layoutqueue::Run();
    }
    g_running = false;
}

namespace layoutqueue {
void Enqueue(HostState* host) {
    g_layoutQueue.push_back(host);
}

void Remove(HostState* host) {
    std::erase(g_layoutQueue, host);
}

bool HasWork() {
    return !g_layoutQueue.empty();
}

void Run() {
    if (g_layoutRunning || g_layoutQueue.empty()) return;
    g_layoutRunning = true;

    // Requests made during the run go to the next pass.
    auto batch = std::move(g_layoutQueue);
    g_layoutQueue.clear();

    std::vector<HostState*> roots;
    for (auto* host : batch) {
        host->layoutQueued = false;
        auto* root = host->LayoutRoot();
        if (std::find(roots.begin(), roots.end(), root) == roots.end()) roots.push_back(root);
    }

    for (auto* root : roots) {
        if (!root->IsActiveInHierarchy()) {
            root->layoutParked = true;
            continue;
        }
        try {
            root->RecalculateLayoutImmediate();
        } catch (std::exception const& e) {
            ReactiveLogger.error("Reactive: layout recalculation threw: {}", e.what());
        } catch (...) {
            ReactiveLogger.error("Reactive: layout recalculation threw an unknown exception");
        }
    }
    g_layoutRunning = false;
}
}
}
