# EggsizerML: A Tool for Automated Fish Egg Measurement

## Project Overview:
EggsizerML is an open-source software project aimed at automating the measurement of fish egg sizes using unsupervised deep learning techniques. This tool leverages edge detection algorithms to measure fish egg sizes from microscope images, eliminating the need for manually labeled data. It is designed to aid marine biologists and environmental scientists in precise measurements, supporting ecological assessments, breeding studies, and species conservation efforts.

## Features:
- **Automated Edge Detection**: Uses unsupervised deep learning models to detect edges and measure egg sizes without manual calibration.
- **Data Export**: Measurement data can be exported in CSV format for further analysis.
- **Results Verification**: Visual feedback with confidence levels for each measurement allows users to verify the accuracy of the results.
- **Species Detection**: Differentiates species of fish eggs based on size variations.
- **Batch Processing**: Supports bulk image uploads for large-scale analysis in laboratory settings.

## Project Structure:
The structure of the project is organized to facilitate development and scalability:

src/ - Source files for the application, organized by different components. 
lib/ - C++ source files for the application logic (.cpp, .hpp files). 
include/ - Header files for the application library (.h files). 
tests/ - Test files for the application, potentially organized into subdirectories. 
doc/ - Documentation files related to the project. 
models/ - (To be added) Folder for machine learning models and training data when applicable.

## Branching Model:
The project follows a simple and effective branching strategy:
- Each feature is developed in its own branch.
- All feature branches will be based off the latest version of the `main` branch.
- Features must be integrated back into the `main` branch before advancing to a new product version.
- Each feature branch will contain all changes related to that feature and will be merged as a complete unit.

## Code Review Process:
The team utilizes a GitHub project board to track tasks and manage the review process. Key points include:
- Every pull request (PR) must be reviewed and approved by all team members before merging into `main`.
- Merges will only occur once all team members are aware and have signed off, ensuring that issues are easily traceable.
- Automated tests must pass for each PR before approval.
- Code reviews focus on ensuring alignment with project goals and preventing any issues that could break the codebase.

## Requirements:
- C++: The project is implemented in C++ to leverage the performance benefits, especially with libraries like OpenCV.
- Qt: Used for building the GUI, providing an easy-to-use and flexible framework.
- CMake: For cross-platform build compatibility.
- OpenCV: For edge detection and image processing.
- mlpack: For implementing machine learning algorithms.

## Setup Instructions:
1. Clone the repository:
    ```
    git clone https://github.com/yourusername/eggsizerML.git
    ```
2. Navigate to the project directory:
    ```
    cd eggsizerML
    ```
3. Build the project using CMake:
    ```
    mkdir build
    cd build
    cmake ..
    make
    ```
4. Run the application:
    ```
    ./EggsizerML
    ```

## Contribution Guidelines:
- Fork the repository and create a new branch for your feature or bug fix.
- Make sure to update relevant tests and documentation.
- Submit a pull request for review once your changes are ready.
- All PRs must pass automated tests and be approved by other team members before merging.

## License:
This project is licensed under the MIT License
