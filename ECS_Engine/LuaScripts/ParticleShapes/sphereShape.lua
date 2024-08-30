params = {
    radius = 1.0
}

display_name = "Sphere"

function get_shape_data()
    local u = math.random()
    local v = math.random()

    local theta = u * 2.0 * math.pi;
    local phi = math.acos(2.0 * v - 1.0)

    local sinTheta = math.sin(theta)
    local cosTheta = math.cos(theta)

    local sinPhi = math.sin(phi)
    local cosPhi = math.cos(phi)

    local x = params.radius * sinPhi * cosTheta
    local y = params.radius * sinPhi * sinTheta
    local z = params.radius * cosPhi

    local pos = {x, y, z}

    return pos, pos
end
