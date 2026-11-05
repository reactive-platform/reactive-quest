# Reactive for Quest

[![Build](https://github.com/reactive-platform/reactive-quest/actions/workflows/build.yml/badge.svg)](https://github.com/reactive-platform/reactive-quest/actions/workflows/build.yml)

# About

Reactive is a UI library for Beat Saber mods. You describe a tree of components, bind them to states, and [Yoga](https://www.yogalayout.dev/) lays them out with Flexbox.

This repo is the Quest port of [reactive-mono](https://github.com/reactive-platform/reactive-mono), in C++. It targets Beat Saber 1.40.8 on Scotland2.

# Adding it to a mod

Reactive isn't on QPM yet, so a mod builds against a local copy:

1. [Compile](#compiling) Reactive, then add it to your local QPM cache from this repo:
   ```
   qpm install ./build/libreactive.so ./build/debug/libreactive.so --locked
   ```
2. Add the dependency to your mod's `qpm.json` and run `qpm restore`:
   ```json
   { "id": "reactive", "versionRange": "^0.1.0", "additionalData": {} }
   ```
3. Compile your mod as C++23 with NDK r27. The components rely on deducing `this`.

> **⚠️** Reactive has to load as a late mod, because its setup runs in `late_load`. Install `Reactive.qmod`, or push `libreactive.so` to `Modloader/mods`. From `Modloader/libs` the setup doesn't run and the UI comes up empty.

# Example

A label and a button that share a state:

```cpp
#include <string>

#include "reactive/beatsaber/BsTextButton.hpp"
#include "reactive/beatsaber/Label.hpp"
#include "reactive/layout/Layout.hpp"

using namespace reactive;
using namespace reactive::beatsaber;

class CounterPanel : public ReactiveComponent {
protected:
    UnityEngine::GameObject* Construct() override {
        auto count = Remember(0);

        return (new Layout({
            .flexController = {
                .flexDirection = FlexDirection::Column,
                .alignItems    = Align::Center,
                .gap           = YogaVector::All(1.f),
            },
            .children = {
                new Label({
                    .sText = count->Map([](int x) { return "Pressed " + std::to_string(x) + " times"; }),
                }),

                new BsTextButton({
                    .flexItem = {.size = {20.f, 8.f}},
                    .onClick  = [count] { count->SetValue(count->Value() + 1); },
                    .text     = "Press",
                }),
            },
        }))->Use();
    }
};
```

To show it, for example from a view controller, use it on a parent. It fills the parent's rect:

```cpp
(new CounterPanel())->Use(get_transform());
```

# Components

| Header | Contents |
|---|---|
| `reactive/layout/Layout.hpp` | `Layout`, the flex container |
| `reactive/beatsaber/Label.hpp` | `Label` |
| `reactive/beatsaber/Image.hpp`, `Background.hpp` | `Image`, and `Background`, an image that can hold children |
| `reactive/beatsaber/WebImage.hpp` | `WebImage`, an image loaded from a URL |
| `reactive/beatsaber/BsButton.hpp`, `BsTextButton.hpp` | Buttons in the game's style |
| `reactive/beatsaber/ScrollArea.hpp`, `Scrollbar.hpp` | A scrollable area and its scrollbar, linked by a `ScrollContext` |
| `reactive/beatsaber/Spinner.hpp` | `Spinner` |
| `reactive/components/Repeater.hpp` | `Repeater`, a cell for each item in a list |
| `reactive/pool/ReactivePool.hpp` | `ReactivePool` |

The rest of PC's components, like input fields and dropdowns, aren't ported yet.

# Coming from PC

Every component takes one `Props` struct, so designated initializers stand in for C#'s object initializers, and the Roslyn generators aren't needed:

| C# | C++ |
|---|---|
| `new Label { Text = "Hi" }` | `new Label({.text = "Hi"})` |
| `sColor = color` | `.sColor = color` |
| `FlexItem = { Size = new() { y = 8.pt } }` | `.flexItem = {.size = {.y = 8_pt}}` |
| `Do = x => x.WithPointerEvents(...)` | `->WithPointerEvents({...})` |
| `.Bind(ref _label)` | `->Bind(_label)` |
| `ContentTransform = { pivot = ... }` | `x->ContentTransform()->set_pivot(...)` |

These behave differently:

- **Ownership.** Create components with `new` and don't delete them: the GameObject owns them, and destroying it deletes them. A callback that can outlive its component should capture `LifetimeToken()`, which takes the place of `IsDestroyed`:
  ```cpp
  DownloadAsync([this, lifetime = LifetimeToken()](Result result) {
      if (lifetime.expired()) return;
      Show(result);
  });
  ```
- **Construction.** C++ base constructors can't call the derived `Construct()`, so it runs on first need, such as `Use()` or `With()`. Setters and props work before that.
- **Extension methods.** Include `reactive/layout/LayoutExtensions.hpp` to chain `AsFlexItem` or `AsFlexGroup`, and `reactive/components/ComponentExtensions.hpp` for `WithPointerEvents`, `AsRectMask` or `WithAlpha`.
- **Required props.** Nothing enforces them at compile time, so leaving out something like `ScrollArea`'s `.scrollContext` fails at runtime.
- **Images.** `WebImage` loads static images from URLs. GIFs and local files aren't supported.

# Compiling

```
git clone --recurse-submodules https://github.com/reactive-platform/reactive-quest
cd reactive-quest
qpm restore
qpm ndk resolve -d
qpm s build
qpm s qmod
```

`qpm ndk resolve -d` downloads a matching NDK, and writing an NDK's path to `ndkpath.txt` works too. The `qpm s` scripts need PowerShell 7. Without it, build with CMake, which also needs Ninja:

```
cmake -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -B build
cmake --build ./build
```

The stripped library ends up in `build/libreactive.so`, the one with symbols in `build/debug/libreactive.so`, and the mod in `Reactive.qmod`.

## Updating Yoga

Yoga is a submodule in `lib/yoga`, on the version [yoga-binding](https://github.com/reactive-platform/yoga-binding) uses for PC. Its public headers are also copied to `shared/yoga`, because a QPM package only ships `shared`. When you update the submodule, copy them again. CI fails if they differ.

# Releasing

Every push is built by CI, and the outputs are attached to the run. Pushing a tag like `v0.2.0` also takes the version from the tag and creates a GitHub release with `libreactive.so`, `debug_libreactive.so` and `Reactive.qmod`.

# License

[MIT](LICENSE)
