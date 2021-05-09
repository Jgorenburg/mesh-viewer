# mesh-viewer

OPenGL applications for viewing meshes

## How to build

*Windows*

Open git bash to the directory containing this repository.

```
mesh-viewer $ mkdir build
mesh-viewer $ cd build
mesh-viewer/build $ cmake -G "Visual Studio 16 2019" ..
mesh-viewer/build $ start CS312-MeshViewer.sln
```

Your solution file should contain multiple projects, such as `mesh-viewer`.
To run from the git bash command shell, 

```
mesh-viewer/build $ ../bin/Debug/mesh-viewer
```

*macOS*

Open terminal to the directory containing this repository.

```
mesh-viewer $ mkdir build
mesh-viewer $ cd build
mesh-viewer/build $ cmake ..
mesh-viewer/build $ make
```

To run each program from build, you would type

```
mesh-viewer/build $ ../bin/mesh-viewer
```

# TODO: Screenshots and writeup

My unique feature for this project is per-pixel phong lighting, which allows for much more realistic and detailed reflectiveness in models.

Vertex Phong:                                                                     

![vTri](/images/triangle2.gif)

Pixel Phong:

![pTri](/images/triangle.gif)


Other models:

![ant](/images/ant.gif)

![doug](/images/douglass.gif)



