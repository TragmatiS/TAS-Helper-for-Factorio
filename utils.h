#pragma once

#include <string>
#include <vector>

static const std::vector<std::string> item_categories = {"Logistics", "Production", "Intermediates", "Combat"};
static const std::vector<std::string> item_categories_fuel = { "Fuel"};

static const std::vector<std::string> item_logistics = {"Wooden chest", "Iron chest", "Steel chest"};
static const std::vector<std::string> item_production = { "Boiler", "Steam engine", "Burner mining drill", "Electric mining drill"};
static const std::vector<std::string> item_intermediates = { "Wood", "Coal", "Stone", "Iron ore", "Copper ore"};
static const std::vector<std::string> item_combat = { "Pistol", "Submachine gun", "Shotgun"};

static const std::vector<std::string> item_fuels = {"Wood", "Coal", "Solid fuel", "Rocket fuel", "Nuclear fuel", "Uranium fuel cell"};

static const std::vector<std::string> build_directions = {"North", "South", "East", "West"};

struct furnace_types {
	std::string stone = "stone-furnace";
};

struct drill_types {
	std::string burner = "burner-mining-drill";
	std::string electric = "electric-mining-drill";
};

struct transport_types {
	std::string yellow_belt = "transport-belt";
};

struct fuel_types {
	std::string wood = "wood";
	std::string coal = "coal";
};

struct take_from_inventory {
	std::string chest = "defines.inventory.chest";
	std::string furnace = "defines.inventory.furnace_result";
	std::string drill = "defines.inventory.fuel";
	std::string lab = "defines.inventory.lab_input";
	std::string assembly = "defines.inventory.assembling_machine_input";
};

struct plate_types {
	std::string iron = "iron-plate";
	std::string copper = "copper-plate";
};

struct ore_types {
	std::string stone = "stone";
	std::string iron = "iron-ore";
	std::string copper = "copper-ore";
};

struct intermediate_types {
	std::string iron_gear_wheel = "iron-gear-wheel";
	std::string copper_cable = "copper-cable";
	std::string circuit_green = "electronic-circuit";
};

struct pipe_struct {
	std::string pipe = "pipe";
	std::string pipe_ground = "pipe-to-ground";
};

struct power_structures_struct {
	std::string boiler = "boiler";
	std::string steam_engine = "steam-engine";
	std::string pump = "offshore-pump";
	std::string small_pole = "small-electric-pole";
};

struct chest_types {
	std::string wood = "wooden-chest";
};

struct assembly_types {
	std::string level_1 = "assembling-machine-1";
	std::string level_2 = "assembling-machine-2";

};

struct science_struct {
	std::string lab = "lab";
	std::string red_science = "automation-science-pack";
	std::string green_science = "logistic-science-pack";
};

struct build_distance_struct {
	float one = 9.8f;
	float two = 8.8f;
	float three = 6.8f;
	float four = 5.8f;
	float five = 3.8f;
};

struct mining_distance_struct {
	float one = 2.7f;
	float two = 1.7f;
	float three = 0.7f;
};

struct build_direction_struct {
	std::string north = "defines.direction.north";
	std::string south = "defines.direction.south";
	std::string east = "defines.direction.east";
	std::string west = "defines.direction.west";
};

struct row_direction_struct {
	std::string north = "north";
	std::string south = "south";
	std::string east = "east";
	std::string west = "west";

};

struct inserter_types {
	std::string yellow = "inserter";
};

struct building_size_struct {
	int small_building = 1;
	int medium_building = 2;
	int large_building = 3;
	int very_large_building = 7;
};

enum walk_direction {
	north = 1,
	northeast = 2,
	northwest = 3,
	south = 4,
	southeast = 5,
	southwest = 6,
	east = 7,
	west = 8
};