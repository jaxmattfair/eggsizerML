![Build Status](https://github.com/jaxmattfair/eggsizerML/actions/workflows/build.yml/badge.svg?branch=release)
[![Latest Version](https://badge.fury.io/gh/jaxmattfair%2FeggsizerML.svg)](https://badge.fury.io/gh/jaxmattfair%2FeggsizerML)

# Eggsizer: Automated Fish Egg Measurement

## Table of Contents
- [Releases](#releases)
- [Introduction](#introduction)
- [Features](#features)
- [Usage](#usage)
- [Development and Contribution Guidelines](#development-and-contribution-guidelines)
- [Contribution Cheatsheet](#contribution-cheatsheet)
- [Development Model and Overview](#development-model-and-overview)
- [Dev Environment Setup](#dev-environment-setup)
- [Project Structure](#project-structure)
- [Project Components](#project-components)
- [License](#license)

## Releases:

Releases follow Semantic Versioning (SemVer), and the latest release can be found on the [Releases Page](https://github.com/jaxmattfair/eggsizerML/releases/latest)

## Introduction:

Eggsizer is an open-source software project that automates measurement of fish eggs using classical computer vision methods. This tool leverages multiple object and edge detection algorithms to measure fish eggs from microscope images, eliminating the need for manual measurements (often done by-hand using tools like [ImageJ](https://imagej.net/ij/)). It is designed to aid marine biologists and environmental scientists in precise measurements, supporting ecological assessments, breeding studies, and species conservation efforts and reducing the time and effort required for manual measurements while also improving accuracy and reproducibility.

## Features:

- **Input Data Preprocessing**: Images are converted to openCV matrices and grayscaled prior to running through the two main algorithms, detailed below.

  - **Blob Detection**: Using openCV's [SimpleBlobDetector](https://docs.opencv.org/3.4/d0/d7a/classcv_1_1SimpleBlobDetector.html), the image is binarized repeatedly using a variety of parameters and pixels are summarily grouped together by area, circularity, and inertia. The resulting blobs are then filtered by size and scaled according to user-input for the microscope image's pixel:millimeter ratio.
  - **Otsu's Thresholding**: Using openCV's [threshold](https://docs.opencv.org/4.x/d7/d4d/tutorial_py_thresholding.html) function in conjunction with [Otsu's method](https://en.wikipedia.org/wiki/Otsu%27s_method) for image thresholding, the image is binarized and the resulting contours are approximated into polygons. The resulting polygons are then filtered by size and scaled according to user-input for the microscope image's pixel:millimeter ratio.

- **Kalman Filter Estimation**: A Kalman filter is used to estimate the size of the fish eggs based on the detected blobs and contours, providing a measurement that balances the strengths and weaknesses of each individual algorithm.
- **Live Image Display**: Live display of processed images is provided so that users can choose to save, discard, or manually adjust image analysis parameters prior to saving results.
- **Data Export**: Measurement data can be exported in both CSV and JSON format for easy feed-forward into other applications, and contoured images can optionally also be saved for future lab-analysis.
- **CLI and GUI**: The application provides both a command-line interface (CLI) and a graphical user interface (GUI) to allow users to tailor their usage to the format most conducive to their workflow.
- **Batch Processing**: The application supports bulk-image processing that allows users to select large datasets of images to analyze in a single run with low-latency.

## Usage:

STILL NEED TO FILL THIS IN.

## Development and Contribution Guidelines:

### Contribution Cheatsheet:

- Branch from `develop` for new features and bug fixes, following the branch naming convention detailed below.
- Create tests where relevant for new features and bug fixes, and ensure all tests pass and are incorporated into the existing test suite prior to submitting a pull request.
- When ready, submit a pull request to `develop` for review. Include a description of the changes made and any relevant issue numbers, and mark at least one other contributor on the repository for review.
- Once approved, perform a squash merge into `develop`, delete the feature branch, and close related issues.

### Development Model and Overview:

Project development follows a simple Git branching model:

- `develop` is the main development branch where all features and bug fixes are integrated. This branch is regularly synced with `main` by the core development team.
- `main` is the stable branch that contains the latest release of the project. Stable user-facing releases should always come from this branch.
- Feature branches are created from `develop` for each new feature or bug fix, and each feature/bug fix should have a related issue on the repository with relevant tags and descriptions, linked to the project board.
- Branch naming convention is to retain the Github-generated branch name (including the issue number) pre-pended with an emoji as dictated by [Carlos Cuesta's Emoji Guide](https://gitmoji.dev/). Common ones include:
  - 🐛 (:bug:) for bug fixes
  - ✨ (:sparkles:) for new features
  - 👷 (:construction_worker:) for build and pipeline adjustments

### Dev Environment Setup:

The project uses CMake for cross-platform build compatibility. Prior to building the project, ensure that CMake, OpenCV, and Qt are installed on your system. Configure cmake for your specific build (be sure to specify the Qt and OpenCV versions and executable paths if you have multiple versions installed or do not use the default installation paths).

The core development team recommends using Qt Creator (available free for all platforms under the [Qt Open Source License](https://www.qt.io/download-open-source)) for development and debugging, as it pairs a full-feature IDE with a great GUI builder and is the most compatible with the existing project structure. _However_, many devs prefer the modern powerhouse [VSCode](https://code.visualstudio.com/), which is also compatible with this project (although we recommend you at least install the relevant Qt extensions for VSCode to make your development life easier).

### Project Structure:

The structure of the project is organized to facilitate modular development and scalability:

`src/` - Source files for the application, organized into standalone modules that are callable on their own for modularity. Application logic is driven by `src/main.cpp`, which hands control over to `eggsizerml.cpp` if the CLI is not called.

`include/` - Header files for the application, containing declarations for the classes and functions used in the source files.

`tests/` - Test files for the application, containing unit tests for the various modules and functions.

`doc/` - Directory containing the Doxygen-generated documentation for the project.

## Project Components:

- C++: The project is implemented in C++ to leverage the performance benefits, especially with libraries like OpenCV.
- Qt: Used for building the GUI, providing an easy-to-use and flexible framework.
- CMake: For cross-platform build compatibility.
- OpenCV: For edge detection and image processing.
- QMake: For rendering as wasm and building for web-based applications.

## License:

This project is licensed under the MIT License
