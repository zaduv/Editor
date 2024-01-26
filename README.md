# 3D drawing editor

This repository contains one of my most recent projects, a 3D drawing editor. 
The main difficulty was to efficiently update the data in real time by doing stream compaction, since a lot of cells can be empty.
Most of the complexity is hidden inside the shaders because the GPU is used as much as possible rather than the CPU.
It can render any kind of scalar field, for example, it could render a torus using its signed distance field.
The grids and the three colored disks are rendered analytically to avoid aliasing.

https://github.com/zaduv/Editor/assets/151663578/8f69f479-3a6a-43a6-9ccf-2225b18ee735

# Other projects

I recorded some footage of some other projects I did in the past. I experimented with skeletal animation and different types of shading, as seen here with the obvious cell shading.
The code is not well maintained as the features piled up on one another over time. It is the predecessor to the 3D drawing editor.

https://github.com/zaduv/Editor/assets/151663578/933e0b3e-3f93-4a21-8f9b-db6f1abe13b0

This a older project, from back when I was first learning about computer graphics. It showcases a simple terrain with shadow mapping.

https://github.com/zaduv/Editor/assets/151663578/6aa7869a-fb8a-41ac-a1b6-7977a2e37198
