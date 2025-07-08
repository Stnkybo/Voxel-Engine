# SDL3PRJ
A Simple 3D Application Using SDL3 and OpenGL to Draw voxels to the screen

![img_1.png](readmeImages/img_1.png)
![img_2.png](readmeImages/img_2.png)
![img_3.png](readmeImages/img_3.png)


## Dependencies
### General 
* Ensure Python is installed and added to PATH to compile some dependencies
* Ensure you have CMake min version 3.22
### Windows
* Ensure that Ninja is installed 
* Compile with CMake:
  * cmake -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..
### Linux
* Ensure OpenGL is installed for your system (if your using linux, you can figure it out.)
  * For Ubuntu: sudo apt install libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev

Other than that, it should install and download all other dependencies when cmake is run