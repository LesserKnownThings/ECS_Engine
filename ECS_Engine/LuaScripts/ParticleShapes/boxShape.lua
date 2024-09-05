params = {
    width = 10.0,
    height = 1.0,
    length = 10.0
}

display_name = "Box"

function get_shape_data()
    local x = math.random() * params.width
    local y = math.random() * params.height
    local z = math.random() * params.length

    return {x, y, z}, {0.0, y, 0.0}
end
