#pragma once

#include "OpenTas.h"

void OpenTas::reset()
{
	segments.reserve(100);

	return_data = {};

	return_data.success = false;
}

open_file_return_data* OpenTas::Open(DialogProgressBar* dialog_progress_bar, std::ifstream& file)
{
	reset();

	dialog_progress_bar->set_text("Opening file");
	dialog_progress_bar->set_button_enable(false);
	dialog_progress_bar->set_progress(0);
	dialog_progress_bar->Show();

	if (!extract_total_steps(file) || !extract_goal(file))
	{
		return &return_data;
	}

	Category category = extract_steps(file, dialog_progress_bar);
	switch (category)
	{
		case Invalid:
			return &return_data;

		case Group:
			if (!extract_groups(file, dialog_progress_bar))
			{
				return &return_data;
			}

			// fallthrough so tamplate is also done, if groups are in the save file
		case Template:
			if (!extract_templates(file, dialog_progress_bar))
			{
				return &return_data;
			}

			break;

		default:
			break;
	}

	if (extract_save_location(file) &&
		extract_script_location(file) &&
		extract_auto_close(file) &&
		extract_auto_put(file))
	{
		return_data.success = true;
	}

	if (update_segment(file))
	{
		return_data.selected_rows = wxGridBlockCoordsVector();
		for (int i = 1; i < segments.size(); i += 2)
		{
			return_data.selected_rows.push_back(
				wxGridBlockCoords(stoi(segments[i]), 0, stoi(segments[i + 1]), 9)
			);
		}
	}

	if (!extract_log_config(file))
		return_data.success = false;

	if (!extract_generate_config(file))
		return_data.success = false;

	return &return_data;
}

bool OpenTas::extract_total_steps(std::ifstream& file)
{

	if (!update_segment(file) || segments[0] != total_steps_indicator)
	{
		return false;
	}

	if (!update_segment(file) || !is_number(segments[0]))
	{
		return false;
	}

	total_lines = std::stoi(segments[0]);

	return true;
}

bool OpenTas::extract_goal(std::ifstream& file)
{
	if (!update_segment(file) || segments[0] != goal_indicator)
	{
		return false;
	}

	if (!update_segment(file) || segments.size() != 1)
	{
		return false;
	}

	return_data.goal = segments[0];

	return true;
}

Category OpenTas::extract_steps(std::ifstream& file, DialogProgressBar* dialog_progress_bar)
{
	if (!update_segment(file) || (segments[0] != steps_indicator && segments[0] != "Tasks:"))
	{
		return Invalid;
	}

	return_data.steps.reserve(total_lines);

	buildingSnapshot.reserve(100000);
	for (int i = 0; i < buildingSnapshot.capacity(); i++)
	{
		buildingSnapshot.emplace_back(invalidX);
	}

	int buildingsInSnapShot = 0;
	int position = 0;

	return_data.warnings_states_counters = WarningsStatesCounters();

	while (update_segment(file))
	{
		const size_t segment_size = segments.size();
		if (segment_size != step_segment_size &&
			segment_size != step_segment_size_without_colour &&
			segment_size != step_segment_size_without_comment_and_colour &&
			segment_size != step_segment_size_without_comment_and_colour_and_modifiers)
		{
			if (segment_size >= 0 && segments[0] == save_templates_indicator)
			{
				return Template;
			}
			else if (segment_size >= 0 && segments[0] == save_groups_indicator)
			{
				return Group;
			}
			else
			{
				return Invalid;
			}
		}

		StepParameters step(invalidX, 0);

		if (segments[1] != "")
		{
			step.X = stod(segments[1]);
			step.OriginalX = step.X;
			step.Y = stod(segments[2]);
			step.OriginalY = step.Y;
		}

		step.Amount = Capitalize(segments[3]);
		step.Item = Capitalize(segments[4], true);
		step.orientation = Capitalize(segments[5]);
		step.Direction = MapStringToOrientation(segments[6]);
		step.Size = segments[7] != "" ? stoi(segments[7]) : 1;
		step.Buildings = segments[8] != "" ? stoi(segments[8]) : 1;
		step.Comment = segment_size == step_segment_size || segment_size == step_segment_size_without_colour ? segments[9] : "";
		step.colour = segment_size == step_segment_size && segments[10] != "" ? wxColour(segments[10]) : wxNullColour;
		step.Modifiers.FromString(segment_size == step_segment_size ? segments[11] : "");

		try
		{
			step.type = ToStepType(segments[0]);
		}
		catch (...)
		{
			if (segments[0] == "Start" || segments[0] == "start") continue; // Ignore start steps, given that they are obsolete.
			else return Invalid;
		}

		switch (step.type)
		{
			case e_build:
				step.BuildingIndex = BuildingNameToType[step.Item];
				step.OrientationEnum = MapStringToOrientation(step.orientation);

				buildingsInSnapShot = ProcessBuildStep(buildingSnapshot, buildingsInSnapShot, step);
				break;

			case e_mine:
				ProcessMiningStep(buildingSnapshot, buildingsInSnapShot, step);
				break;

			case e_priority:
				step.priority.FromString(step.orientation);
				step.orientation = "";

				// Only here to populate extra parameters in step. Actual validation will be done on script generation
				BuildingExists(buildingSnapshot, buildingsInSnapShot, step);
				break;

			case e_recipe:
			case e_filter:
			case e_rotate:
			case e_launch:
				// Only here to populate extra parameters in step. Actual validation will be done on script generation
				BuildingExists(buildingSnapshot, buildingsInSnapShot, step);
				break;

			case e_limit:
			case e_put:
			case e_take:
				step.inventory = GetInventoryType(step.orientation);
				// Only here to populate extra parameters in step. Actual validation will be done on script generation
				BuildingExists(buildingSnapshot, buildingsInSnapShot, step);
				break;

			case e_never_idle:
				return_data.warnings_states_counters.never_idle++;
				break;
			case e_keep_on_path:
				return_data.warnings_states_counters.keep_on_path++;
				break;
			case e_keep_crafting:
				return_data.warnings_states_counters.keep_crafting++;
				break;
			case e_keep_walking:
				return_data.warnings_states_counters.keep_walking++;
				break;

			default:
				break;
		}

		return_data.steps.push_back(step);

		lines_processed++;

		if (lines_processed > 0 && lines_processed % 250 == 0)
		{
			dialog_progress_bar->set_progress(static_cast<float>(lines_processed) / static_cast<float>(total_lines) * 100.0f - 50);
			wxYield();
		}
	}

	return Invalid;
}

bool OpenTas::extract_groups(std::ifstream& file, DialogProgressBar* dialog_progress_bar)
{
	vector<StepParameters> steps = {};
	string name = "";
	int position = 0;

	// Groups are obsolete and have been moved to template. This is here for backwards compatibility. 
	while (update_segment(file))
	{
		if (segments.size() != group_segment_size && segments.size() != group_segment_size_without_comment)
		{
			if (segments.size() == 1 && segments[0] == save_templates_indicator)
			{
				if (name != "")
				{
					return_data.template_map.insert(pair<string, vector<StepParameters>>(name, steps));
				}

				return true;
			}

			return false;
		}

		if (name == "")
		{
			name = segments[0];
			steps = {};

		}
		else if (name != segments[0])
		{
			return_data.template_map.insert(pair<string, vector<StepParameters>>(name, steps));

			name = segments[0];
			steps = {};
		}

		string comment = "";
		if (segments.size() == group_segment_size)
		{
			comment = segments[10];
		}

		StepParameters step(invalidX, 0);

		if (segments[2] != "")
		{
			step.X = stod(segments[2]);
			step.OriginalX = stod(segments[2]);
			step.Y = stod(segments[3]);
			step.OriginalY = stod(segments[3]);
		}

		if (segments[8] != "")
		{
			step.Size = stoi(segments[8]);
		}

		if (segments[9] != "")
		{
			step.Buildings = stoi(segments[9]);
		}

		step.Amount = Capitalize(segments[4]);
		step.Item = Capitalize(segments[5], true);
		step.orientation = Capitalize(segments[6]);
		step.Direction = MapStringToOrientation(segments[7]);
		step.Comment = comment;

		try
		{
			step.type = ToStepType(segments[1]);
		}
		catch (...)
		{
			if (segments[1] == "Start" || segments[1] == "start") continue; // Ignore start steps, given that they are obsolete.
			else return Invalid;
		}

		switch (step.type)
		{
			case e_build:
				step.BuildingIndex = BuildingNameToType[step.Item];
				step.OrientationEnum = MapStringToOrientation(step.orientation);
				break;

			case e_priority:
				step.priority.FromString(step.orientation);
				step.orientation = "";
				break;

			case e_limit:
			case e_put:
			case e_take:
				step.inventory = GetInventoryType(step.orientation);
				break;
			default:
				break;
		}

		steps.push_back(step);

		lines_processed++;

		if (lines_processed > 0 && lines_processed % 25 == 0)
		{
			dialog_progress_bar->set_progress(static_cast<float>(lines_processed) / static_cast<float>(total_lines) * 100.0f - 47);
			wxYield();
		}
	}

	return false;
}

bool OpenTas::extract_templates(std::ifstream& file, DialogProgressBar* dialog_progress_bar)
{
	vector<StepParameters> steps = {};
	string name = "";
	int position = 0;

	while (update_segment(file))
	{
		if (segments.size() != template_segment_size &&
			segments.size() != template_segment_size_without_colour &&
			segments.size() != template_segment_size_without_comment_and_colour)
		{
			if (segments.size() == 1 && segments[0] == save_file_indicator)
			{
				if (name != "")
				{
					if (return_data.template_map.find(name) != return_data.template_map.end())
					{
						name += "_Template";
					}

					return_data.template_map.insert(pair<string, vector<StepParameters>>(name, steps));
				}

				return true;
			}

			return false;
		}

		if (name == "")
		{
			name = segments[0];
			steps = {};
		}
		else if (name != segments[0])
		{
			if (return_data.template_map.find(name) != return_data.template_map.end())
			{
				name += "_Template";
			}

			return_data.template_map.insert(pair<string, vector<StepParameters>>(name, steps));

			name = segments[0];
			steps = {};
		}

		string comment = "";
		if (segments.size() == template_segment_size)
		{
			comment = segments[10];
		}

		StepParameters step(invalidX, 0);

		if (segments[2] != "")
		{
			step.X = stod(segments[2]);
			step.OriginalX = stod(segments[2]);
			step.Y = stod(segments[3]);
			step.OriginalY = stod(segments[3]);
		}

		if (segments[8] != "")
		{
			step.Size = stoi(segments[8]);
		}

		if (segments[9] != "")
		{
			step.Buildings = stoi(segments[9]);
		}

		
		step.Amount = Capitalize(segments[4]);
		step.Item = Capitalize(segments[5], true);
		step.orientation = Capitalize(segments[6]);
		step.Direction = MapStringToOrientation(segments[7]);
		step.Comment = comment;

		try
		{
			step.type = ToStepType(segments[1]);
		}
		catch (...)
		{
			if (segments[1] == "Start" || segments[1] == "start") continue; // Ignore start steps, given that they are obsolete.
			else return Invalid;
		}

		switch (step.type)
		{
			case e_build:
				step.BuildingIndex = BuildingNameToType[step.Item];
				step.OrientationEnum = MapStringToOrientation(step.orientation);
				break;

			case e_priority:
				step.priority.FromString(step.orientation);
				step.orientation = "";
				break;

			case e_limit:
			case e_put:
			case e_take:
				step.inventory = GetInventoryType(step.orientation);
				break;
			default:
				break;
		}

		steps.push_back(step);

		lines_processed++;

		if (lines_processed > 0 && lines_processed % 25 == 0)
		{
			dialog_progress_bar->set_progress(static_cast<float>(lines_processed) / static_cast<float>(total_lines) * 100.0f - 45);
			wxYield();
		}
	}

	return false;
}

bool OpenTas::extract_save_location(std::ifstream& file)
{
	if (!update_segment(file) || segments.size() != 1)
	{
		return false;
	}

	return_data.save_file_location = segments[0];
	return true;
}

bool OpenTas::extract_script_location(std::ifstream& file)
{
	if (!update_segment(file) || segments[0] != code_file_indicator && segments[0] != "Task folder location:")
	{
		return false;
	}

	if (!update_segment(file) || segments.size() != 1)
	{
		return false;
	}

	return_data.generate_code_folder_location = segments[0];
	return true;
}

bool OpenTas::extract_auto_close(std::ifstream& file)
{
	if (!update_segment(file) || segments[0] != auto_close_indicator)
	{
		return false;
	}

	if (!update_segment(file) || segments.size() != 2 || segments[0] != auto_close_generate_script_text)
	{
		return false;
	}

	return_data.auto_close_generate_script = segments[1] == "true";

	if (!update_segment(file) || segments.size() != 2 || segments[0] != auto_close_open_text)
	{
		return false;
	}

	return_data.auto_close_open = segments[1] == "true";

	if (!update_segment(file) || segments.size() != 2 || segments[0] != auto_close_save_text)
	{
		return false;
	}

	return_data.auto_close_save = segments[1] == "true";

	if (!update_segment(file) || segments.size() != 2 || segments[0] != auto_close_save_as_text)
	{
		return false;
	}

	return_data.auto_close_save_as = segments[1] == "true";

	return true;
}

bool OpenTas::extract_auto_put(std::ifstream& file)
{
	if (!update_segment(file) || segments[0] != auto_put_indicator)
	{
		return false;
	}

	if (!update_segment(file) || segments.size() != 2 || segments[0] != auto_put_furnace_text)
	{
		return false;
	}

	return_data.auto_put_furnace = segments[1] == "true";

	if (!update_segment(file) || segments.size() != 2 || segments[0] != auto_put_burner_text)
	{
		return false;
	}

	return_data.auto_put_burner = segments[1] == "true";

	if (!update_segment(file) || segments.size() != 2 || segments[0] != auto_put_lab_text)
	{
		return false;
	}

	return_data.auto_put_lab = segments[1] == "true";

	if (!update_segment(file) || segments.size() != 2 || segments[0] != auto_put_recipe_text)
	{
		return false;
	}

	return_data.auto_put_recipe = segments[1] == "true";

	return true;
}

bool OpenTas::extract_generate_config(std::ifstream& file)
{
	if (!update_segment(file)) // logconfig doesn't exist so default
	{
		return_data.generateConfig = {
			.legacy_mining = false,
		};
		return true;
	}
	else if (segments[0] != generate_indicator)
	{
		return false;
	}
	size_t s = segments.size();
	return_data.generateConfig = {
		.legacy_mining = s < 2 || segments[1] == "1" ? true : false,
	};
	return true;
}

bool OpenTas::extract_log_config(std::ifstream& file)
{
	if (!update_segment(file)) // logconfig doesn't exist so default
	{
		return_data.logconfig = {
			.savegame = true,
			.tech = true,
			.comment = true,
			.level = log_config::leveltype::DEV,
		};
		return true;
	}
	else if (segments[0] != logging_indicator)
	{
		return false;
	}
	size_t s = segments.size();
	return_data.logconfig = {
		.savegame = s < 2 || segments[1] == "1" ? true : false,
		.tech = s < 3 || segments[2] == "1" ? true : false,
		.comment = s < 4 || segments[3] == "1" ? true : false,
		.level = (log_config::leveltype) (s < 5 ? 1 : std::stoi(segments[4])),
	};
	return true;
}

bool OpenTas::update_segment(std::ifstream& file)
{
	string line;
	string segment;
	std::stringstream data_line;

	if (std::getline(file, line))
	{
		data_line.str(line);

		segments.clear();
		segments.reserve(16);

		while (std::getline(data_line, segment, ';'))
		{
			segments.push_back(segment);
		}

		return true;
	}

	return false;
}