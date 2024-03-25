#include"../game_headers/Inventory.h"
#include"../game_headers/Pickup.h"

void Inventory::Add(Pickup* item)
{
	items.push_back(item);
}

void Inventory::Clear()
{
	for (int i = 0; i < items.size(); i++)
	{
		delete items[i];
	}
}