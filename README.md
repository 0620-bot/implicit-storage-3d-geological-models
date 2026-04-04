implicit-storage-3d-geological-models

Source code and example materials for the paper "An Implicit Storage Method for 3D Geological Models Supporting Multiscale Modelling and Cross-Sectional Analysis"


# Runnable software and demo video

Runnable software packages and the demonstration video are available in the GitHub Releases section:

**Release v1.0.0:**  
https://github.com/0620-bot/implicit-storage-3d-geological-models/releases/tag/v1.0.0


# Implicit Storage Method for 3D Geological Models

This repository contains the source code, documentation, and example materials for the paper:
"An Implicit Storage Method for 3D Geological Models Supporting Multiscale Modelling and Cross-Sectional Analysis"


## Overview

This software implements an implicit storage and cross-sectioning workflow for 3D geological models.

Instead of storing only explicit mesh results, the method stores the implicit functions and modelling metadata of geological interfaces, enabling:

- multiscale model reconstruction at different meshing resolutions;
- efficient cross-section generation directly from stored implicit functions.

The current implementation is developed in C++14 using the Qt 5.9.1 framework.



## Repository contents

- `Code/` - C++ source code. include UI files, qrc file, .h files, .cpp files
- The runnable software package and demonstration video had been provided in the GitHub Releases section



## Main functions

- Read stored implicit geological model data
- Reconstruct 3D geological models at user-specified mesh resolutions
- Generate geological cross-sections based on implicit surfaces



## Third-party dependencies

This software depends on Qt 5.9.1.

Users should comply with the license terms of Qt and any other third-party libraries used in this project.


## Citation

If you use this code in academic work, please cite the corresponding paper.


## Contact

For questions regarding the code and paper, please contact:

Zhibin Liu  Email: liuzhibin0620@qq.com

Jiateng Guo*  Email: guojiateng@mail.neu.edu.cn