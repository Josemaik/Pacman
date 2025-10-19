
scorePowerUp = 300
--powerUpTime = 7
--speedMultiplier = 2
coin_score = 25
--bronzeThreshold = 80

-- Crea el pacman principal
mainPacman = Pacman.new(1.5,5,2.0) --live,poweruptime,speed

function ComputeMedals(score)
    local medals = {}

    local bronces_para_plata = pacman.getBroncesparaPlata()
    local platas_para_oro = pacman.getPlatasparaOro()

    local bronce = score % bronces_para_plata
    local plata_total = math.floor(score / bronces_para_plata)
    local plata = plata_total % platas_para_oro
    local oro = math.floor(plata_total / platas_para_oro)

    medals.bronce = bronce
    medals.plata = plata
    medals.oro = oro

    return medals
end

function RestartGame()
    mainPacman:setLives(1.5)
end

function PowerUpGone()
    mainPacman:setColor(255, 0, 0)
    mainPacman:setSpeed(1.0)
end

function PacmanEaten()
    local vida = mainPacman:getLives()
    vida = vida - 0.5
    mainPacman:setLives(vida)
    return vida < 0.0
end

function addCoin()
    local num_coins = pacman.getNumCoins()
    num_coins = num_coins + 1
    pacman.setNumCoins(num_coins);
    mainPacman.addScore(num_coins * coin_score)
end

function getPowerUpColor(vidas)
    local color = {}

    if vidas == 1.5 then
        color.r = 255
        color.g = 0
        color.b = 0
    elseif vidas == 1.0 then
        color.r = 255
        color.g = 165
        color.b = 0
    elseif vidas == 0.5 then
        color.r = 0
        color.g = 255
        color.b = 0
    elseif vidas == 0.0 then
        color.r = 0
        color.g = 0
        color.b = 255
    else
        color.r = 255
        color.g = 255
        color.b = 255
    end

    return color
end

function applyPowerUp()
    local vidas = mainPacman:getLives()
    local color = getPowerUpColor(vidas)
    mainPacman:setSpeed(2.0)
    mainPacman:setPowerUpDuration()
    mainPacman:setColor(color.r, color.g, color.b)
    pacman.addScore(scorePowerUp)
end