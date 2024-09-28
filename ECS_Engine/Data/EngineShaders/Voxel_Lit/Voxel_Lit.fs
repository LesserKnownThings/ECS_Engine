#version 450

uniform vec3 cameraPos = vec3(0, 0, 0);
uniform vec2 screenResolution = vec2(1920, 1080);

uniform int visibleObjects = 0;
uniform int voxelsCount = 0;

const int maxAttempts = 8;

out vec4 FragColor;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

struct Voxel
{
    uint mortonId;
    uint colorId;
};

struct VoxelNode
{
    bool isLeaf;
    uint children[8];
    uint voxelIndex;
};

struct AABB
{
    vec3 min;
    vec3 max;
};

struct Object
{
    mat4 model;
    AABB boundary;
};

layout(std430, binding = 0) buffer Octree
{
    VoxelNode octree[];
};

layout(std430, binding = 1) buffer OctreeData
{
    Voxel voxels[];
};

layout(std430, binding = 2) buffer Objects
{
    Object objects[];
};

const float t = 2000;

bool Intersects(vec3 rayOrigin, vec3 rayDir, vec3 boxMin, vec3 boxMax, float t_max)
{
    vec3 t1 = (boxMin - rayOrigin) / rayDir;
    vec3 t2 = (boxMax - rayOrigin) / rayDir;

    vec3 t_min = min(t1, t2);
    vec3 t_max_vec = max(t1, t2);

    float t_near = max(max(t_min.x, t_min.y), t_min.z);
    float t_far  = min(min(t_max_vec.x, t_max_vec.y), t_max_vec.z);

    if (t_near < t_far && t_far > 0.0 && t_near <= t_max)
    {
        return true;
    }
    return false;
}

void main()
{
    vec3 rayOrigin = cameraPos;

    vec2 ndc = (gl_FragCoord.xy / screenResolution) * 2.0 - 1.0;
    vec4 cliSpace = vec4(ndc, -1.0, 1.0);
    vec4 viewSpace = inverse(projection) * cliSpace;
    viewSpace /= viewSpace.w;

    vec3 rayDirViewSpace = normalize(viewSpace.xyz);
    vec3 rayDirWorldSpace = normalize((inverse(view) * vec4(rayDirViewSpace, 0.0)).xyz);
    
    vec3 color = vec3(1.0);

    for(int i = 0; i < visibleObjects; ++i)
    {
        Object obj = objects[i];
        if(Intersects(rayOrigin, rayDirWorldSpace, obj.boundary.min, obj.boundary.max, t))
        {
            color = vec3(0, 1, 0);
        }
    }

    FragColor = vec4(color, 1.);
}