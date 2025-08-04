Study Sync Pro
==========================

A desktop application that joins hardworking university students together who are working on similar assignments and courses, enabling them to form intuitive study groups and collaborate effectively to develop, adapt, and overcome educational challenges.

# Table of Contents

* About
* Features
* Prerequisites
* Installation
* Building and Running
* Usage

# About

Study Sync Pro is a C++ application built with SFML that addresses the challenges university students face when looking for study partners. The platform connects students who are working on similar subjects or struggling with the same topics, providing an alternative perspective from peers and fostering collaborative learning.

This project was developed for CEN3031 Summer 2025,

Made with  ***Love***, by Justin, Yeshua, Luis, & Mason.

# Features

* **Profile Management**: Create and update personal profiles with academic interests and subjects
* **Smart Group Matching**: Algorithm-based matching system that connects students based on:
     * Common subjects and courses
     * Academic interests
* **Study Group Discovery**: Browse recommended study groups with members and descriptions
* **Modern GUI**: Clean, intuitive interface built with SFML

# Prerequisites

Before building Study Sync Pro, ensure you have the following installed:
1. C++ Compiler: MinGW-w64 (GCC 7.3.0 or higher)
     * Verify with: g++ --version
2. SFML 2.5.1: Simple and Fast Multimedia Library
     * Download from: https://www.sfml-dev.org/download/sfml/2.5.1/
     * Default installation path: C:\SFML-2.5.1
3. Make: GNU Make for building the project
     * Usually comes with MinGW as mingw32-make
4. Git: For version control
     * Download from: https://git-scm.com/

# Installation

1. Install SFML:
* Download SFML 2.5.1 for your compiler (MinGW)
* Extract to C:\SFML-2.5.1
* If installed elsewhere, update the path in makefile
2. Ensure mingw32 is downloaded to C drive on your personal computer
2. Download the zip via the Github
* Github: https://github.com/JustinSui624/Study-Sync-Pro.git
3. Open .zip
4. Extract .zip to desired location
5. Open newly extracted .zip folder and locate "required_libraries"
6. Extract "required_libraries" to main folder of "Study Sync Pro" alongside other folders such as src, fonts, exactly
7. Open terminal, (right click inside file explorer and select "Open in Terminal")

# Building and Running
Following the installation steps for Study Sync Pro, you will eventually find yourself in the terminal. Here is how you properly run the program in the terminal:
1. While in "\Study-Sync-Pro-main>" folder, run:  **mingw32-make**  
* This command will build the project 
2. After waiting for the command to finish processing, type:  **.\bin\StudySyncPro.exe**
3. Now, enjoy the application! :)


# Usage
First Time Users
------------------------
1. Launch the application
2. Click "Create Account" to create a new account
3. Fill in all required fields:
     * Full name
     * Graduation year  
     * Preferred subjects
     * Username
     * Password
4. Click "Register" to create your account
Returning Users
------------------------
1. Enter your username and password
2. Click "Login"
Setting Up Your Profile
------------------------
1. After logging in, you'll see the Profile page
2. Add your subjects/topics for the groups you would like to join:
     * Example: mathematics, chemestry, english
3. Enter your graduation year
4. Click "Save Profile"
5. Click "Find Groups"
Finding Study Groups
------------------------
1. After selecting, "Find Groups," our application will show recommended study groups based on:
* Your listed subjects
3. Groups display:
* Group name
* Description
* Related subjects
Navigation
------------------------
* **Login Page**: Initial screen 
* **Registration Page**: Create new accounts
* **Profile Page**: Manage your information and interests
* **Group Matching Page**: View recommended study groups
* **Logout**: Return to login screen

# Contributing

1. Create a feature branch: git checkout -b feature-name
2. Commit changes: git commit -am 'Add new feature'
3. Push to branch: git push origin feature-name
4. Submit a Pull Request






# CEN3031-S16799-Group-4
[![CodeFactor](https://www.codefactor.io/repository/github/justinsui624/study-sync-pro/badge)](https://www.codefactor.io/repository/github/justinsui624/study-sync-pro)
# License
Study-Sync-Pro use the Apache License 2.0 (https://github.com/JustinSui624/Study-Sync-Pro/blob/main/LICENSE)
# Required Libraries
SFML Minimum Required Version:
https://www.sfml-dev.org/download/sfml/2.5.1/
# Team Name:
Study Group Task Force
# Team Members: 
Justin Sui,
Yeshua Colon,
Luis Borruel,
Mason Bush
# Project Management
Trello: https://trello.com/invite/b/683c8be760d394c9922e5183/ATTIb493c9b5b26e635391e89151552ad3e4928E9D3B/project-board
# About
For this project, our team will create an application that can connect students that are working/struggling on similar assignments/problems.This platform can help students who are stuck to get an alternative view from peers.


