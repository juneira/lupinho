function init()
    x = 200
    t = 0
end

function update()
    x = x + 4
    t = t + 0.05

    y =  math.sin(t) * 25

    ui.draw_text("Bem-vindo ao Lupi!", 280, 180 + math.floor(y))
end
