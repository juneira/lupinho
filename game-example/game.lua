require "palette"
require "sprites"

x = 200
t = 0
player_x = 100
player_y = 100
frame = 0
frame_counter = 0

for i = 1, #Palette do
    ui.palset(i - 1, Palette[i])
end

function update()
    x = x + 4
    t = t + 0.05

    y =  math.sin(t) * 25

    ui.draw_text("Bem-vindo ao Lupi!", 280, 180 + math.floor(y))

    -- ui.draw_line(10, 10, 280, 180, 3)
    -- ui.draw_rect(50, 50, 80, 80, true, 2)

    -- ui.draw_rect(185, 185, 50, 50, false, 5)
    -- ui.draw_circle(200, 300, 20, true, 6, true, 10)
    -- ui.draw_circle(260, 300, 20, true, 7, false, 11)
    -- ui.draw_circle(200, 360, 20, false, 8, true, 12)
    -- ui.draw_triangle(20, 250, 100, 250, 55, 350, 9)

    ui.spr(SpriteSheets['tilemap.leaves'], 10, 50)


    ui.tile(SpriteSheets['player.run.' .. frame + 1], 0, player_x, player_y)

    frame_counter = frame_counter + 1
    if frame_counter >= 5 then
        frame = (frame + 1) % 5
        frame_counter = 0
    end

    if ui.btn(RIGHT, 0) then
        player_x = player_x + 3
    end
    if ui.btn(LEFT, 0) then
        player_x = player_x - 3
    end
    if ui.btn(UP, 0) then
        player_y = player_y - 3
    end
    if ui.btn(DOWN, 0) then
        player_y = player_y + 3
    end

    if ui.btnp(BTN_Z, 0) then
        ui.draw_text("BTN_Z", 200, 100)
    elseif ui.btnp(BTN_E, 0) then
        ui.draw_text("BTN_E", 200, 100)
    elseif ui.btnp(BTN_Q, 0) then
        ui.draw_text("BTN_Q", 200, 100)
    end
end
