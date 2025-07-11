#include "item_tracker.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

// TODO:
// Case insensitive search for names, std::transform and std::tolower changes everything to lowercase.
// frequency_map_[toLower(item)]++;

/* Color helper unnamed namespace I can re-use throughout this file */
namespace
{
	constexpr auto color_red = "\033[31m"; //constexpr means evaluated at compile time.
	constexpr auto color_green = "\033[32m";
	constexpr auto reset_color = "\033[0m";
}

/* This handles the data stream reading and writing */
void ItemTracker::loadData()
{
	frequency_map_.clear(); //clear frequency map before reading data into it.

	std::ifstream in;
	in.exceptions(std::ios::badbit); //exception handling types to check for upon open file
	in.open(input_file_name_); //read input_file_name_ with in object.
	if (!in) //if the stream isn't true, error handling for opening file
	{
		throw std::runtime_error("Could not open " + input_file_name_);
		/* Handled by main's try/catch */
	}

	std::string item;
	//while (true)
	//{
	//	in >> item; //sets failbit/eofbit only after a read attempt fails.
	//	if (in.fail()) //if fail/eof, break
	//	{
	//		break;
	//	}
	//	frequency_map_[item]++; //otherwise increment adding each item to map
	//} 

	/* in >> item is a boolean conversion that iterates over the file while the stream returns good/true. It'll end at end of file.
	 * >> treats whitespace as a separator and drops it, so it does each word one by one.
	 * could also do while (getline(in, item)) but that fetches the entire line.
	 */
	while (in >> item)
	{
		frequency_map_[item]++; // Adds each item and key to frequency map
	}
	writeBackup(); // Writes the frequency map data to the backup file.
}

/* These are the menu options -- exit is handled within the main loop itself in menu_control */
int ItemTracker::getItemCount(const std::string& item) const //Menu 1
{
	//Looked up and saw that instead of using .at, .find is safer.
	auto item_count = frequency_map_.find(item);
	if (item_count != frequency_map_.end()) //.end is the position after the last element, a sentinel value.
	{
		//key found, access value: item_count->second
		return item_count->second; //first is the key name, second is the value or total frequency of the name.
	}
	return 0; //Not found.
}

void ItemTracker::displayAllFrequencies() const //Menu 2
{
	/* Creating a pair variable assigned with each map element, first is the name, second is the value */
	for (const auto& pair : frequency_map_) 
	{
		std::cout << std::setw(15) << std::left << color_green << pair.first << reset_color << ": "
		<< color_red << pair.second << reset_color << '\n';
	}
}

void ItemTracker::displayHistogram(char symbol) const //Menu 3, default symbol argument is '*'
{
	// TODO:
	// Auto scale histogram bar length, e.g. std::max_element
	// add visual display as an x,y chart? Sort option: vector, pair, sort 

	/* Guard for empty visual data */
	
	if (frequency_map_.empty())
	{
		std::cout << "No frequency data loaded.\n";
		return;
	}
	/* How to handle symbol length being too long ?
	 * std::string(pair.second, symbol) builds a string of pair.second copies of symbol.
	 * first is the name, second is the value
	 */
	for (const auto& pair : frequency_map_)
	{ // Symbol_length makes max symbol at 16 *
		int symbol_length = std::min(pair.second, 16); //min returns the smaller of (a,b)
		std::cout << std::setw(15) << std::left << color_green << pair.first << reset_color << ' '
		<< color_red << std::string(symbol_length, symbol) << reset_color << " (" << pair.second << ")\n";
		
	}
}

/* Menu 4 extra option I added: To call this, just do tracker_.setFilename("NewTextFile.txt"); */
void ItemTracker::setFileName(const std::string& file_name)
{
	input_file_name_ = file_name; //update file name
	loadData(); // reload data
}

/* Write a backup of the map data to frequency.dat */
void ItemTracker::writeBackup() const
{   /* trunc removes the files old content upon opening, so it only writes the current data to back up. */
	std::ofstream write_backup(backup_file_name_, std::ios::trunc);
	if (!write_backup) //if the stream isn't true, error handling for opening file
	{
		std::cerr << color_red << "Could not write to " + backup_file_name_ << reset_color << '\n';
		return;
 	}
	for (const auto& pair : frequency_map_)
	{
		write_backup << pair.first << ' ' << pair.second << '\n';
	}
}

