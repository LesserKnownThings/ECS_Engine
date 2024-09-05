params = {
    radius = 1.0,
    height = 1.0,
    angle = 45.0
}

display_name = "Cone"

function get_shape_data()
    local small_height = params.height / 10.0
    local trunc_height = params.height - small_height

    local half_angle = params.angle / 2.0

    local hyp = params.height / math.cos(math.rad(half_angle))
    local hyp_small = small_height / math.cos(math.rad(half_angle))

    local y = small_height + (params.height - small_height) * math.random()

    local big_radius = math.sqrt(hyp * hyp - params.height * params.height) * 2.0 * params.radius
    local small_radius = math.sqrt(hyp_small * hyp_small - small_height * small_height) * 2.0 * params.radius

    local r = small_radius + (big_radius - small_radius) * (y / trunc_height)

    local theta = math.random() * 2.0 * math.pi

    local x = r * math.cos(theta)
    local z = r * math.sin(theta)

    local pos = {x, y, z}

    local vel = {0 - x, params.height - y, 0 - z}

    return pos, pos
end
