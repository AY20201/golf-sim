#ifndef INVENTORY_CLASS_H
#define INVENTORY_CLASS_H

#include<vector>

class Pickup;

class Inventory
{
public:

	std::vector<Pickup*> items;
	void Add(Pickup* item);
	void Clear();
};

#endif
