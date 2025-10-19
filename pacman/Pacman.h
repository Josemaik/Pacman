#pragma once

struct Pacman {
  float speedMultiplier = 1.0f;
  int powerUpTime = 7;
  int colorR = 255, colorG = 255, colorB = 255;

  float max_lives = 1.5f;
  float lives = max_lives;

  void setSpeed(float speed);
  float getSpeed() const;
  void setColor(int r, int g, int b);
  void setPoweUpTime(int poweruptime);
  int getPowerUpTime() const;
  void setLives(float v);
  float getLives() const;
};