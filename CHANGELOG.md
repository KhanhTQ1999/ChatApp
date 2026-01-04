# Changelog

All notable changes to this project will be documented in this file.

The format is based on "Keep a Changelog" and follows Semantic Versioning.

## [Unreleased]
### Added
- Added test method.
- Integrated QT UI.
- Add communicattion over both LAN and Internet

### Changed
- No changes.

### Fixed
- Fixed parsing issue when messages contain spaces in the `send` command.

## [0.1.0] - 2026-01-04
### Added
- Initial public release of ChatApp (peer-to-peer console chat).
- Core features:
  - TCP socket-based peer-to-peer messaging.
  - Console UI with commands: help, myip, myport, connect, list, send, terminate, exit.
  - Concurrent connection handling using threads.
  - Modular project layout: UI/view, controller, model, services, utils.
  - CMake build system (C++17).
- Example usage and basic troubleshooting notes in README.

### Fixed
- No fixes.

### Changed
- No changes.