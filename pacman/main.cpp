#include <pacman_include.hpp>
#include "lua.hpp"
#include "Pacman.h"

//Lua State
lua_State* L = nullptr;
//fordward declaration
void reloadLuaScript();
void registerPacmanClass(lua_State* L);

//global variables
const int platas_para_oro = 5;
static int bronces_para_plata = 100;

//static int powerUpTime = 5;
//static float speedMultiplier = 2.f;

//int pacmanR, pacmanG, pacmanB = 255;

int num_coins = 0;
int currentScore = 0;
static int PowerUpScore = 100;
static int coinScore = 50;

//const float max_vida = 1.5f;
float vida = 1.5f;

//////////////////////////////////////////////////
//Lua Functions to be registered
// 
// Método de registro
static const char* PACMAN_META = "Pacman.Metatable";

// Método new: Pacman.new()
static int PacmanNew(lua_State* L) {
	float live = lua_tonumber(L, 1);
	int powerUpTime = lua_tointeger(L, 2);
	float speed = lua_tonumber(L, 3);
	Pacman* new_pacman = (Pacman*)lua_newuserdata(L, sizeof(Pacman));
	new_pacman->setLives(live);
	new_pacman->setPoweUpTime(powerUpTime);
	new_pacman->setSpeed(speed);

	luaL_getmetatable(L, PACMAN_META);
	lua_setmetatable(L, -2);
	return 1;
}


// Métodos
static int PacmanaddScore(lua_State* L) {
	int extrascore = luaL_checkinteger(L, 1);
	currentScore = extrascore;
	return 0;
}

static int PacmansetSpeed(lua_State* L) {
	Pacman* p = (Pacman*)lua_touserdata(L, 1);
	p->setSpeed(luaL_checknumber(L, 2));
	//float speed = luaL_checknumber(L, 1);
	setPacmanSpeedMultiplier(p->getSpeed());
	return 0;
}

static int PacmansetColor(lua_State* L) {
	Pacman* p = (Pacman*)lua_touserdata(L, 1);
	p->setColor(
		luaL_checkinteger(L, 2),
		luaL_checkinteger(L, 3),
		luaL_checkinteger(L, 4));
	setPacmanColor(p->colorR, p->colorG, p->colorB);
	return 0;
}

int PacmansetPowerUpDuration(lua_State* L)
{
	Pacman* p = (Pacman*)lua_touserdata(L, 1);
	int duration = p->getPowerUpTime();
	setPowerUpTime(duration);
	return 0;
}

static int PacmansetScore(lua_State* L) {
	int extraScore = luaL_checkinteger(L, 1);
	currentScore = extraScore;
	return 0;
}

static int PacmangetScore(lua_State* L) {
	lua_pushinteger(L, currentScore);
	return 1;
}

static int PacmangetLives(lua_State* L) {
	Pacman* p = (Pacman*)lua_touserdata(L, 1);
	lua_pushnumber(L, p->getLives());
	return 1;
}

static int PacmansetLives(lua_State* L) {
	Pacman* p = (Pacman*)lua_touserdata(L, 1);
	p->setLives(luaL_checknumber(L, 2));
	vida = p->getLives();
	return 0;
}

static int PacmangetNumCoins(lua_State* L)
{
	lua_pushnumber(L, num_coins);
	return 1;
}

static int PacmansetNumCoins(lua_State* L)
{
	num_coins = luaL_checkinteger(L, 1);
	return 0;
}

int PacmangetBroncesparaPlata(lua_State* L)
{
	lua_pushnumber(L, bronces_para_plata);
	return 1;
}

int PacmangetPlatasparaOro(lua_State* L)
{
	lua_pushnumber(L, platas_para_oro);
	return 1;
}

// Métodos disponibles con obj:method()
static const struct luaL_Reg Pacman_methods[] = {
		{"addScore", PacmanaddScore},
		{"setSpeed", PacmansetSpeed},
		{"setColor", PacmansetColor},
		{"setScore", PacmansetScore},
		{"getScore", PacmangetScore},
		{"getLives", PacmangetLives},
		{"setLives", PacmansetLives},
		{"getNumCoins", PacmangetNumCoins},
		{"setNumCoins", PacmansetNumCoins},
		{"getBroncesparaPlata", PacmangetBroncesparaPlata},
		{"getPlatasparaOro", PacmangetPlatasparaOro},
		{"setPowerUpDuration", PacmansetPowerUpDuration},
		{NULL, NULL}
};

// Funciones del módulo Pacman (Pacman.new)
static const struct luaL_Reg Pacman_funcs[] = {
		{"new", PacmanNew},
		{NULL, NULL}
};

void registerPacmanClass(lua_State* L) {

	luaL_newmetatable(L, PACMAN_META);

	lua_pushvalue(L, -1); // duplicate metatable
	lua_setfield(L, -2, "__index");

	luaL_register(L, NULL, Pacman_methods);
	luaL_register(L,"Pacman", Pacman_funcs);
}

//////////////////////////////////////////////////////


//////////////////////////////////////////////////////
//Pacman Callbacks
//
bool pacmanEatenCallback(int& score, bool& muerto)
{ // Pacman ha sido comido por un fantasma
	lua_getglobal(L, "PacmanEaten");
	if (lua_isfunction(L, -1)) {
		int error = 0;
		error |= lua_pcall(L, 0, 1, 0);
		if (error) {
			fprintf(stderr, "Error en PacmanEaten: %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
		}
		if (lua_isboolean(L, -1))
			muerto = lua_toboolean(L, -1);
	}

	return true;
}

bool coinEatenCallback(int& score)
{ // Pacman se ha comido una moneda
	lua_getglobal(L, "addCoin");
	if (lua_isfunction(L, -1)) {
		int error = 0;
		error |= lua_pcall(L, 0, 0, 0);
		if (error) {
			fprintf(stderr, "Error en addCoin: %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	score = currentScore;
	return true;
}

bool frameCallback(float time)
{ // Se llama periodicamente cada frame
	return false;
}

bool ghostEatenCallback(int& score)
{ // Pacman se ha comido un fantasma
	return false;
}

bool powerUpEatenCallback(int& score)
{ 
	// Pacman se ha comido un powerUp
	lua_getglobal(L, "applyPowerUp");
	if (lua_isfunction(L, -1)) {
		int error = 0;
		error |= lua_pcall(L, 0, 0, 0);
		if (error) {
			fprintf(stderr, "Error en applyPowerUp: %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else {
		lua_pop(L, 1);
	}
	score = currentScore;
	return true;
}

bool powerUpGone()
{ 
	lua_getglobal(L, "PowerUpGone");
	if (lua_isfunction(L, -1)) {
		int error = 0;
		error |= lua_pcall(L, 0, 0, 0);
		if (error) {
			fprintf(stderr, "Error en PowerUpGone: %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
	else {
		lua_pop(L, 1);
	}
	return true;
}

bool pacmanRestarted(int& score)
{
	//if (!L) return false;

	//reloadLuaScript();

	lua_getglobal(L, "RestartGame");
	int type = lua_type(L, -1);
	printf("RestartGame is of type: %s\n", lua_typename(L, type));
	if (lua_isfunction(L, -1)) {
		int error = 0;
		error |= lua_pcall(L, 0, 0, 0);
		if (error) {
			fprintf(stderr, "Error en Restart: %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
			return false;
		}
	}
	else {
		lua_pop(L, 1);
		return false;
	}
	
	score = 0;
	num_coins = 0;

	return true;
}

bool computeMedals(int& oro, int& plata, int& bronce, int score)
{
	if (!L) return false;
	lua_getglobal(L, "ComputeMedals");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 1);
		fprintf(stderr, "ComputeMedals no está definida\n");
		return false;
	}
	// push score
	lua_pushnumber(L, score);

	// call function
	if (lua_pcall(L, 1, 1, 0) != 0) {
		fprintf(stderr, "Error en ComputeMedals: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1); // pop error
		return false;
	}

	
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return false;
	}

	lua_getfield(L, -1, "plata");
	if (lua_isnumber(L, -1))
		plata = lua_tointeger(L, -1);
	else
		plata = 0;
	lua_pop(L, 1);

	lua_getfield(L, -1, "bronce");
	if (lua_isnumber(L, -1))
		bronce = lua_tointeger(L, -1);
	else
		bronce = 0;
	lua_pop(L, 1);

	lua_getfield(L, -1, "oro");
	if (lua_isnumber(L, -1))
		oro = lua_tointeger(L, -1);
	else
		oro = 0;
	lua_pop(L, 1);

	lua_pop(L, 1); // pop la tabla completa
	return true;
}

bool getLives(float& vidas)
{
	vidas = vida;
	return true;
}

bool setImmuneCallback()
{
    return true;
}

bool removeImmuneCallback()
{
    return true;
}

bool InitGame()
{
	L = luaL_newstate();
	luaL_openlibs(L);

	// REGISTRAR PACMAN
	registerPacmanClass(L);

	// Ejecutar Lua principal
	int error = luaL_loadfile(L, "lua/pacman.lua");
	error |= lua_pcall(L, 0, 0, 0);
	if (error) {
		fprintf(stderr, "%s", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	lua_getglobal(L, "scorePowerUp");
	if (lua_isnumber(L, -1))
		PowerUpScore = lua_tointeger(L, -1);
	lua_pop(L, 1);

	lua_getglobal(L, "coin_score");
	if (lua_isnumber(L, -1))
		coinScore = lua_tointeger(L, -1);
	lua_pop(L, 1);

  return true;
}

bool EndGame()
{
	if (L) lua_close(L);
    return true;
}

//////////////////////////////////////////////////////////////////////

void reloadLuaScript()
{
	if (!L) return;

	int error = luaL_loadfile(L, "lua/pacman.lua");
	error |= lua_pcall(L, 0, 0, 0);
	if (error) {
		fprintf(stderr, "Lua error (reload): %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}