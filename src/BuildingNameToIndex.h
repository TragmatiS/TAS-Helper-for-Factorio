#pragma once

#include <map>
#include <string>

using std::map;
using std::string;

enum Assemblers
{
	StoneFurnace = 67,
	SteelFurnace,
	ElectricFurnace,
	AssemblingMachine1,
	AssemblingMachine2,
	AssemblingMachine3,
	OilRefinery,
	ChemicalPlant,
	Centrifuge,
};

static const std::map<string, int> BuildingNameToIndex =
{
	{"Wooden chest", 1},
	{"Iron chest", 2},
	{"Steel chest", 3},
	{"Storage tank", 4},
	{"Transport belt", 5},
	{"Fast transport belt", 6},
	{"Express transport belt", 7},
	{"Underground belt", 8},
	{"Fast underground belt", 9},
	{"Express underground belt", 10},
	{"Splitter", 11},
	{"Fast splitter", 12},
	{"Express splitter", 13},
	{"Loader", 14},
	{"Fast loader", 15},
	{"Express loader", 16},
	{"Burner inserter", 17},
	{"Inserter", 18},
	{"Long-handed inserter", 19},
	{"Fast inserter", 20},
	{"Filter inserter", 21},
	{"Stack inserter", 22},
	{"Stack filter inserter", 23},
	{"Small electric pole", 24},
	{"Medium electric pole", 25},
	{"Big electric pole", 26},
	{"Substation", 27},
	{"Pipe", 28},
	{"Pipe to ground", 29},
	{"Pump", 30},
	{"Curved rail", 31},
	{"Straight rail", 32},
	{"Train stop", 33},
	{"Rail signal", 34},
	{"Rail chain signal", 35},
	{"Locomotive", 36},
	{"Cargo wagon", 37},
	{"Fluid wagon", 38},
	{"Artillery wagon", 39},
	{"Car", 40},
	{"Tank", 41},
	{"Spidertron", 42},
	{"Active provider chest", 43},
	{"Passive provider chest", 44},
	{"Storage chest", 45},
	{"Buffer chest", 46},
	{"Requester chest", 47},
	{"Roboport", 48},
	{"Small lamp", 49},
	{"Arithmetic combinator", 50},
	{"Decider combinator", 51},
	{"Constant combinator", 52},
	{"Power switch", 53},
	{"Programmable speaker", 54},
	{"Boiler", 55},
	{"Steam engine", 56},
	{"Solar panel", 57},
	{"Accumulator", 58},
	{"Nuclear reactor", 59},
	{"Heat pipe", 60},
	{"Heat exchanger", 61},
	{"Steam turbine", 62},
	{"Burner mining drill", 63},
	{"Electric mining drill", 64},
	{"Offshore pump", 65},
	{"Pumpjack", 66},
	{"Stone furnace", 67},
	{"Steel furnace", 68},
	{"Electric furnace", 69},
	{"Assembling machine 1", 70},
	{"Assembling machine 2", 71},
	{"Assembling machine 3", 72},
	{"Oil refinery", 73},
	{"Chemical plant", 74},
	{"Centrifuge", 75},
	{"Lab", 76},
	{"Beacon", 77},
	{"Rocket silo", 78},
	{"Land mine", 79},
	{"Stone wall", 80},
	{"Gate", 81},
	{"Gun turret", 82},
	{"Laser turret", 83},
	{"Flamethrower turret", 84},
	{"Artillery turret", 85},
	{"Radar", 86},
	{"Item on ground", 87},
	{"Rocket silo rocket", 88},
	{"Tile ghost", 89},
	{"Stone brick", 90},
	{"Concrete", 91},
	{"Hazard concrete left", 92},
	{"Hazard concrete right", 93},
	{"Refined concrete", 94},
	{"Refined hazard concrete left", 95},
	{"Refined hazard concrete right", 96},
	{"Landfill", 97},
};

string FindBuildingName(int index)
{
	for (auto it = BuildingNameToIndex.begin(); it != BuildingNameToIndex.end(); ++it)
	{
		if (it->second == index)
		{
			return it->first;
		}
	}

	return "N/A";
}