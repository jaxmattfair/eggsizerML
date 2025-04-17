# EggsizerML: A Tool for Automated Fish Egg Measurement

## Table of Contents
- [Installation](#installation)
- [Image Processing](#image-processing)
- [Exporting Results](#exporting-results)

# Installation

Installing this utility is simple. Head to our [Releases Page](https://github.com/jaxmattfair/eggsizerML/releases/latest), navigate to the latest stable release, and download the zip file for your operating system. Once this zip is downloaded, simply extract it and run the eggsizerML application. We recommend keeping this extracted folder somewhere readily accessible, such as your Desktop or Documents folder.

# Image Processing

EggsizerML will automatically process images using multiple processing techniques. All you have to do is provide the images. This can be done in two ways:

1. The **Open File** button can be used to select a single file, or multiple files to open and process. These images will be loaded into the program and processed individually.
2. The **Open Folder** button can be used to select an entire folder to process. All images in the folder will be loaded into the program and processed.

All images will be automatically processed in the background without you having to do anything. If you want to view the results of the processing techniques for an image, you can use the **Previous Image** and **Next Image** buttons to cycle through the loaded images. Images will be displayed showing how each technique processed them, and the egg size results will be displayed in a table on the right hand side.

# Exporting Results

Exporting results is simple. After loading an image, multiple images, or a folder of images, you can export all results by clicking the **Save Results** button. This will package the results of all loaded images into a CSV formatted file and allow you to name the file and select a location to save it to. 

This CSV file has six columns, as follows:

1. Image Name - The name of the file uploaded
2. Egg No. - The egg number within that uploaded file
3. Avg. Area - The average egg size between Otsu's Area approximation and Blob Area approximation
4. Otsus Area - The calculated egg size using Otsu's Area approximation
5. Blob Area - Blob Area approximation
6. Certainty - A calculated certainty metric for confidence of program results (currently experimental)

CSV files are great as they can be easily processed with many programming languages/applications (Python, R, Matlab), or opened by spreadsheet utilities like Excel and Google Sheets
