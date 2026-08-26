# Changelog

All notable changes to Quantum will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [v1.3.1]

### Added
- Enabled the dropdown element to exceed parent window boundaries without rendering problems.
- Added dynamic connection range determination by checking the file size first to prevent using too many connections on small files resulting in crashes and instabillity.
- Added version label at the main window title bar to indicate the current version of the app.

### Fixed
- Fixed the weak buttons disabling when the link is not valid, preventing starting unexisting downloads.

### Changed
- Removed old classes and ui elements from the QWidgets setup.
- Disabled thread selection dropdown until the download is inserted and Header reply is recieved.

## [v1.3.0]

### Fixed
- Removed Unnecessary show() and raise() on url window open from web integration request.

### Changed
- Made quantum run in system tray instead of closing immediately.

## [v1.2.0]

### Added
- Added firefox integration initial files.
- Added firefox integration redirect logic.

## [v1.1.0]

### Added
- Added chrome extension manifest file.
- Added chrome extension link to the app using local HTTP connection.
