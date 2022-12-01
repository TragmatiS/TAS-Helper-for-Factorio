#pragma once

#include <string>
#include <fstream>
#include <wx/wx.h>

#include "Building.h"
#include "TaskNameToEnum.h"
#include "utils.h"

using std::string;
using std::to_string;
using std::ofstream;

struct StepParameters
{
	double X;
	double Y;
	double OriginalX;
	double OriginalY;
	int Size;
	int Buildings;
	int BuildingIndex;
	Orientation OrientationIndex;

	TaskName TaskEnum;

	string Task;
	string Amount;
	string Item;
	string FromInto;
	string Orientation;
	string Direction;
	string PriorityIn;
	string PriorityOut;
	string Comment;

	StepParameters(double InitialX, double InitialY)
	{
		X = InitialX;
		Y = InitialY;
		OriginalX = InitialX;
		OriginalY = InitialY;

		OrientationIndex = North;
		TaskEnum = e_stop;
		Size = 1;
		Buildings = 1;
		BuildingIndex = 0;
	}

	inline void Reset()
	{
		X = OriginalX;
		Y = OriginalY;
	}

	inline void Next()
	{
		if (Direction == struct_direction_list.north)
		{
			Y -= Size;
			return;
		}

		if (Direction == struct_direction_list.south)
		{
			Y += Size;
			return;
		}

		if (Direction == struct_direction_list.east)
		{
			X += Size;
			return;
		}

		if (Direction == struct_direction_list.west)
		{
			X -= Size;
			return;
		}
	}

	string ToString()
	{
		switch (TaskEnum)
		{

			case e_start:
			case e_pause:
			case e_save:
				return Task + ";" + ";" + ";" + ";" + ";" + ";" + ";" + ";" + ";" + Comment + ";";

			case e_stop:
			case e_game_speed:
			case e_idle:
			case e_pick_up:
				return Task + ";" + ";" + ";" + Amount + ";" + ";" + ";" + ";" + ";" + ";" + Comment + ";";

			case e_build:
				return Task + ";" + to_string(X) + ";" + to_string(Y) + ";" + ";" + Item + ";" + Orientation + ";" + Direction + ";" + to_string(Size) + ";" + to_string(Buildings) + ";" + Comment + ";";

			case e_craft:
				return Task + ";" + ";" + ";" + Amount + ";" + Item + ";" +";" + ";" + ";" + ";" + Comment + ";";

			case e_recipe:
			case e_filter:
				return Task + ";" + to_string(X) + ";" + to_string(Y) + ";" + Amount + ";" + Item + ";" + ";" + Direction + ";" + to_string(Size) + ";" + to_string(Buildings) + ";" + Comment + ";";

			case e_limit:
				return Task + ";" + to_string(X) + ";" + to_string(Y) + ";" + Amount + ";" + ";" + Orientation + ";" + Direction + ";" + to_string(Size) + ";" + to_string(Buildings) + ";" + Comment + ";";

			case e_rotate:
			case e_mine:
				return Task + ";" + to_string(X) + ";" + to_string(Y) + ";" + Amount + ";" + Item + ";" + ";" + ";" + ";" + ";" + Comment + ";";

			case e_priority:
				return Task + ";" + to_string(X) + ";" + to_string(Y) + ";" + ";" + ";" + Orientation + ";" + Direction + ";" + to_string(Size) + ";" + to_string(Buildings) + ";" + Comment + ";";

			case e_put:
			case e_take:
				return Task + ";" + to_string(X) + ";" + to_string(Y) + ";" + Amount + ";" + Item + ";" + Orientation + ";" + Direction + ";" + to_string(Size) + ";" + to_string(Buildings) + ";" + Comment + ";";
			
			case e_launch:
			case e_walk:
				return Task + ";" + to_string(X) + ";" + to_string(Y) + ";" + ";" + ";" + ";" + ";" + ";" + ";" + Comment + ";";

			case e_tech:
				return Task + ";" + ";" + ";" + ";" + Item + ";" + ";" + ";" + ";" + ";" + Comment + ";";

			case e_drop:
				return Task + ";" + to_string(X) + ";" + to_string(Y) + ";" + ";" + Item + ";" + Orientation + ";" + Direction + ";" + to_string(Size) + ";" + to_string(Buildings) + ";" + Comment + ";";
			
			default:
				return Task + ";" + to_string(X) + ";" + to_string(Y) + ";" + Amount + ";" + Item + ";" + Orientation + ";" + Direction + ";" + to_string(Size) + ";" + to_string(Buildings) + ";" + Comment + ";";
		}
	}

	inline bool operator==(const Building& toCompare)
	{
		if (toCompare.X == X && toCompare.Y == Y)
		{
			BuildingIndex = toCompare.Index;
			OrientationIndex = toCompare.OrientationIndex;
			return true;
		}

		return false;
	}

	inline bool operator==(const StepParameters& toCompare)
	{
		if (toCompare.X != X && toCompare.Y != Y)
		{
			return false;
		}

		if (toCompare.X == X && toCompare.Y == Y)
		{
			return true;
		}

		if (toCompare.Buildings < 2)
		{
			return false;
		}

		int amountOfBuildings = toCompare.Buildings;
		int size = toCompare.Size;
		
		if (toCompare.X == X)
		{
			if (toCompare.Direction == struct_direction_list.south)
			{
				if (toCompare.Y > Y)
				{
					return false;
				}

				for (int i = 1; i < amountOfBuildings; i++)
				{
					if (Y == toCompare.Y + i * toCompare.Size)
					{
						return true;
					}
				}
			}

			if (toCompare.Direction == struct_direction_list.north)
			{
				if (toCompare.Y < Y)
				{
					return false;
				}

				for (int i = 1; i < amountOfBuildings; i++)
				{
					if (Y == toCompare.Y - i * toCompare.Size)
					{
						return true;
					}
				}
			}
		}

		if (toCompare.Y == Y)
		{
			if (toCompare.Direction == struct_direction_list.east)
			{
				if (toCompare.X > X)
				{
					return false;
				}

				for (int i = 1; i < amountOfBuildings; i++)
				{
					if (X == toCompare.X + i * toCompare.Size)
					{
						return true;
					}
				}
			}

			if (toCompare.Direction == struct_direction_list.west)
			{
				if (toCompare.X < X)
				{
					return false;
				}

				for (int i = 1; i < amountOfBuildings; i++)
				{
					if (X == toCompare.X - i * toCompare.Size)
					{
						return true;
					}
				}
			}
		}
		
		return false;
	}
};