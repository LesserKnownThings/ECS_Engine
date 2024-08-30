#version 430

layout(local_size_x = 256) in;

struct Particle
{
    vec4 positionLifetime;
    //Since w is not used in velocity I can use it for alpha
    vec4 velocityAlpha;
    //xyz color and w is bool useGravity
    vec4 colorUseGravity;
    vec4 scaleSpawnTime;
    mat4 worldModel;
    bool isLocal;
    bool isActive;
};

struct ParticleInitialData
{
    vec4 positionLifetime;
    vec4 scaleSpawnTime;
};

layout(std430, binding = 0) buffer Particles
{
    Particle particles [];
};

layout(std430, binding = 1) buffer InitialParticles
{
    ParticleInitialData initialParticleValues [];
};

uniform float deltaTime;
uniform mat4 worldModel;

void main()
{
    uint id = gl_GlobalInvocationID.x;

    Particle current = particles[id];
    ParticleInitialData initial = initialParticleValues[id];

    if (!current.isActive)
    {
        current.scaleSpawnTime.w -= deltaTime;
        current.worldModel = worldModel;
    }

    if (current.scaleSpawnTime.w <= 0 && !current.isActive)
    {
        current.isActive = true;
    }

    if (current.isActive)
    {
        current.positionLifetime.xyz += current.velocityAlpha.xyz * deltaTime;
        current.positionLifetime.w -= deltaTime;

        if (current.positionLifetime.w <= 0)
        {
            current.isActive = false;
            current.positionLifetime = initial.positionLifetime;
            current.scaleSpawnTime = initial.scaleSpawnTime;
        }
    }

    particles[id] = current;
}