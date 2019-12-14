# rayTraceEngine
## Polygon Rendering
To allow the renderer to support more complicated geometry, a feature to load and render triangular meshes was added.
Meshes are loaded from a .obj file using basic C++ file io operations. Vertex data is parsed and placed into corresponding Arrays.
Meshes are stored as indexed triangles, where each index is a datastructure with indices for normals, verts, tangents, texcoords, etc.
Ray-Triangle Intersection testing is used for raycasting.
Surface smoothing is accomplished using Phong-Shading, where surface normals are calculated at the fragment stage by linearly interpolating vertex normals [5].

### Accelerating Rendering with Octrees
To accelerate the rendering process, an octree was constructed to organize mesh triangles for faster intersection queries. This octree datastructure utilized a fast Triangle/AABB intersection algorithm for contruction [3], and a fast Ray/AABB [4] intersection test for queries.

## Area Lighting
To improve the visual quality of shadow’s produced by the renderer for objects, area lights was employed. The current deployment uses a naieve approach to area lighting (where several points on the light are sampled per hit). In the future, I would like to move to a more efficient approach (ie. Monte Carlo Method).  
  
These area light’s produce a soft shadow by “sampling” light rays uniformly distributed on the light’s radius, then averaging these values [2].
The result is a penumbra, as samples where the light is partially obscured will yield a lower average intensity than those un-obscured.

## Materials and Texture Maps
Support for texture maps has extended to include specular and bump mapping to provide flexibility. At the moment, bump maps are supported for meshes exclusively.
  
To support bump mapping, the mesh data structure implemented earlier in the semester was modified to also hold tangent data.
Tangents for a mesh are calculated when the mesh is loaded from file using techniques outlined in Foundations of Game Engine Development Volume 2: Rendering [1].
Specular maps allow the reflective properties of a surface to be modulated using a texture.
In addition to specular map support, reflections have also been added to the renderer for Phong shading calculations.
This is accomplished through recursive ray-cast calls along the the direction coincident to the hit direction [5].

## References
[1] Lengyel, E. (2019). Foundations of Game Engine Development Volume 2: Rendering. Lincoln, California: Terathon Software LLC.  
[2] Wenzel, J., Pharr, M., & Humphreys, G. (2004). Physically Based Rendering: From Theory To Implementation. Retrieved from http://www.pbr-book.org/3ed-2018/Light_Transport_I_Surface_Reflection/Sampling_Light_Sources.html  
[3] Akenine-Möllser, T. (2001). Fast 3D Triangle-Box Overlap Testing, Journal of Graphics Tools, 6(1), 29-33, DOI: 10.1080/10867651.2001.10487535  
[4] Williams, A., Barrus, S., Morley, R.K.,  & Shirley, P. (2005). An Efficient and Robust Ray-Box Intersection Algorithm, Journal of graphics tools, 10(1), 49-54.  
[5]  Marschner, S., & Shirley, P. (2016). Fundamentals of Computer Graphics, Fourth Edition. A. K. Peters, Ltd..  


