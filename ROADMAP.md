# Development roadmap

This project was developed in stages, from a minimal application to an interactive
solar system. The eight milestones below preserve the development plan previously
recorded in the [original README](https://github.com/zbybko/solar-system/blob/3216b0ced9e5330c1da5a60a601b6d0658cbd136/README.md),
translated into English. Their completed status comes from that development record;
it does not imply that every configuration has been tested on every platform.

## Original development milestones

### 1. Application skeleton

- [x] Open a 1280 × 720 raylib window and close it with Escape.
- [x] Manage dependencies through vcpkg manifest mode.
- [x] Set up the CMake build.

### 2. UI integration

- [x] Connect Dear ImGui through the rlImGui bridge.
- [x] Render UI panels over the 3D scene.

### 3. Mathematics and ephemeris abstraction

- [x] Implement a mathematical `Vec3` with operator overloads.
- [x] Define `IEphemeris` and implement the `LibnovaEphemeris` adapter.
- [x] Own the provider through `std::unique_ptr<IEphemeris>` and use it
      polymorphically: the Strategy pattern.

### 4. Celestial body hierarchy

- [x] Introduce the abstract `CelestialBody` and its `Star`, `Planet` and `Moon` subclasses.
- [x] Support textured spheres with a colored fallback, axial tilt and rotation.
- [x] Manage model resources through RAII and provide polymorphic `update` / `draw` methods.

Surface maps were bundled during the portfolio refresh below.

### 5. Solar system assembly

- [x] Assemble the Sun, eight planets, Earth's Moon and Saturn's rings in
      `SolarSystem::createRealistic`.
- [x] Track Julian date, simulation speed and pause state in `SimulationClock`.
- [x] Support Cyrillic UI text using DejaVu Sans.

### 6. Camera and interaction

- [x] Implement orbit and free-flight modes in `CameraController`.
- [x] Select bodies by clicking through `SolarSystem::pickBody`.
- [x] Focus on, follow and highlight the selected body.

### 7. Rendering polish

- [x] Draw orbital paths and a starfield.
- [x] Add Sun-based lighting and a day/night terminator using a GLSL shader.
- [x] Encapsulate rendering resources in an RAII-based `Renderer`.
- [x] Switch between compact and real-distance views through `RenderContext::toWorld`.

### 8. Dedicated UI and a second orbital strategy

- [x] Move time, camera, view and selected-body panels into `UIManager`.
- [x] Add `KeplerEphemeris`, using JPL orbital elements without an external astronomy library.
- [x] Allow runtime provider switching through `SolarSystem::setEphemeris`
      without replacing the celestial bodies.

The libnova / Kepler selector is available only when optional libnova support is built.

## Finalization tasks from the original plan

- [ ] Add on-screen body labels using 3D-to-2D projection.
- [ ] Complete a documentation-comment pass over the public interfaces.
- [x] Update the project README — completed during the portfolio refresh below.

## Portfolio refresh — September 2026

These additions came after the original eight-stage plan.

- [x] Add English and German UI translations while retaining Russian.
- [x] Add live language switching and a startup-language setting.
- [x] Keep ImGui widget identities stable across languages.
- [x] Add translation completeness, widget-ID and language-fallback tests.
- [x] Run the localization tests in GitHub Actions.
- [x] Publish English documentation, a German overview and actual application previews.
- [x] Build and capture the English and German application on macOS with the Kepler provider.
- [x] Restore this development plan as a separate, linked document.
- [x] Add a WebAssembly browser build with bundled assets and WebGL lighting.
- [x] Publish the interactive demo on GitHub Pages and automate deployment from `main`.
- [x] Restore the original Sun, planet, Moon and Saturn-ring maps with source credits.
- [x] Correct sphere pole orientation and UV mapping; add a surface-mapping regression test.

## Validation scope

The latest refresh was checked with a macOS application build and startup captures,
plus automated localization tests locally and on GitHub. The browser build was also
checked for scene rendering and live language switching. Windows and Linux graphical
builds and the optional libnova path were not revalidated during that refresh.
Numerical accuracy and the simplified Moon model remain separate concerns; see the
[project limitations](README.md#limitations).
