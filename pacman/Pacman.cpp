#include "Pacman.h"

void Pacman::setSpeed(float speed) { speedMultiplier = speed; }
float Pacman::getSpeed() const{ return speedMultiplier;}
void Pacman::setColor(int r, int g, int b) { colorR = r; colorG = g; colorB = b; }
void Pacman::setPoweUpTime(int poweruptime){ powerUpTime = poweruptime; }
int Pacman::getPowerUpTime() const{ return powerUpTime; }
void Pacman::setLives(float v) { lives = v; }
float Pacman::getLives() const { return lives; }