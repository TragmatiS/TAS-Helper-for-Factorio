#include "StepParameters.h"

#include "../Data/BuildingNames.h"

StepParameters::StepParameters(double InitialX, double InitialY)
{
	X = InitialX;
	Y = InitialY;
	OriginalX = InitialX;
	OriginalY = InitialY;
}

void StepParameters::Reset()
{
	X = OriginalX;
	Y = OriginalY;
}

void StepParameters::Next()
{
	switch (Direction)
	{
		case North: 
			Y -= Size; 
			break;
		case South: 
			Y += Size; 
			break;
		case East: 
			X += Size; 
			break;
		case West: 
			X -= Size; 
			break;
	}
}

string StepParameters::ToString()
{
	const string string_end = ";" + Comment + ";" + colour.GetAsString().ToStdString() + ";" + Modifiers.ToString() + ";";
	const string steptype = StepNames[type];
	switch (type)
	{
		case e_never_idle:
		case e_keep_crafting:
		case e_keep_on_path:
		case e_keep_walking:
		case e_pause:
		case e_save:
			return steptype + ";" + ";" + ";" + ";" + ";" + ";" + ";" + ";" + string_end;

		case e_stop:
		case e_game_speed:
		case e_idle:
		case e_pick_up:
			return steptype + ";" + ";" + ";" + Amount + ";" + ";" + ";" + ";" + ";" + string_end;

		case e_build:
			return steptype + ";" + to_string(X) + ";" + to_string(Y) + ";" + ";" + Item + ";" + orientation + ";" + orientation_list[Direction] + ";" + to_string(Size) + ";" + to_string(Buildings) + string_end;

		case e_craft:
			return steptype + ";" + ";" + ";" + Amount + ";" + Item + ";" + ";" + ";" + ";" + string_end;

		case e_recipe:
		case e_filter:
			return steptype + ";" + to_string(X) + ";" + to_string(Y) + ";" + Amount + ";" + Item + ";" + ";" + orientation_list[Direction] + ";" + to_string(Size) + ";" + to_string(Buildings) + string_end;

		case e_limit:
			return steptype + ";" + to_string(X) + ";" + to_string(Y) + ";" + Amount + ";" + ";" + orientation + ";" + orientation_list[Direction] + ";" + to_string(Size) + ";" + to_string(Buildings) + string_end;

		case e_rotate:
			return steptype + ";" + to_string(X) + ";" + to_string(Y) + ";" + Amount + ";" + Item + ";" + ";" + orientation_list[Direction] + ";" + to_string(Size) + ";" + to_string(Buildings) + string_end;

		case e_mine:
			return steptype + ";" + to_string(X) + ";" + to_string(Y) + ";" + Amount + ";" + Item + ";" + ";" + ";" + ";" + string_end;

		case e_priority:
			return steptype + ";" + to_string(X) + ";" + to_string(Y) + ";" + ";" + ";" + priority.ToString() + ";" + orientation_list[Direction] + ";" + to_string(Size) + ";" + to_string(Buildings) + string_end;

		[[likely]] case e_put:
		[[likely]] case e_take:
			return steptype + ";" + to_string(X) + ";" + to_string(Y) + ";" + Amount + ";" + Item + ";" + inventory_types_list[inventory] + ";" + orientation_list[Direction] + ";" + to_string(Size) + ";" + to_string(Buildings) + string_end;

		case e_launch:
		[[likely]] case e_walk:
			return steptype + ";" + to_string(X) + ";" + to_string(Y) + ";" + ";" + ";" + ";" + ";" + ";" + string_end;

		case e_tech:
			return steptype + ";" + ";" + ";" + ";" + Item + ";" + ";" + ";" + ";" + string_end;

		case e_drop:
			return steptype + ";" + to_string(X) + ";" + to_string(Y) + ";" + ";" + Item + ";" + orientation + ";" + ";" + ";" + string_end;

		case e_shoot:
			return steptype + ";" + to_string(X) + ";" + to_string(Y) + ";" + Amount + ";" + ";" + ";" + ";" + ";" + string_end;
		case e_throw:
			return steptype + ";" + to_string(X) + ";" + to_string(Y) + ";" + ";" + Item + ";" + ";" + ";" + ";" + string_end;

		default:
			return steptype + ";" + to_string(X) + ";" + to_string(Y) + ";" + Amount + ";" + Item + ";" + orientation + ";" + orientation_list[Direction] + ";" + to_string(Size) + ";" + to_string(Buildings) + string_end;
	}
}

bool StepParameters::operator==(const Building& toCompare)
{
	if (toCompare.X == X && toCompare.Y == Y)
	{
		BuildingIndex = toCompare.type;
		OrientationEnum = toCompare.OrientationEnum;
		return true;
	}

	return false;
}

bool StepParameters::operator==(const StepParameters& toCompare)
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
		if (toCompare.Direction == South)
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

		if (toCompare.Direction == North)
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
		if (toCompare.Direction == East)
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

		if (toCompare.Direction == West)
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