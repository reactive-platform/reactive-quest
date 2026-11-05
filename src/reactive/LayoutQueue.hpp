#pragma once

namespace reactive {
struct HostState;

namespace layoutqueue {
void Enqueue(HostState* host);
void Remove(HostState* host);
bool HasWork();
void Run();
}
}
