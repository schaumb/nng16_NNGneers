#include "lakotelep.h"
//#include <utility>
#include <cassert>

void PutOne(std::vector< std::vector<int> >& table, size_t py, size_t px, bool propagate = true)
{
	if (propagate)
	{
		assert(table[py][px] == 0);
		table[py][px] = 1;
		if (py > 0)
			PutOne(table, py - 1, px, false);
		if (py < table.size()-1)
			PutOne(table, py + 1, px, false);
		if (px > 0)
			PutOne(table, py, px - 1, false);
		if (px < table[0].size() - 1)
			PutOne(table, py, px + 1, false);
	}
	else
	{
		if (table[py][px]>0)
		{
			table[py][px] += 1;
			if (table[py][px] == 5)
				table[py][px] = 1;
		}
	}
}

void RemoveOne(std::vector< std::vector<int> >& table, size_t py, size_t px, bool propagate = true)
{
	if (propagate)
	{
		assert(table[py][px] == 1);
		table[py][px] = 0;
		if (py > 0)
			RemoveOne(table, py - 1, px, false);
		if (py < table.size() - 1)
			RemoveOne(table, py + 1, px, false);
		if (px > 0)
			RemoveOne(table, py, px - 1, false);
		if (px < table[0].size() - 1)
			RemoveOne(table, py, px + 1, false);
	}
	else
	{
		if (table[py][px]>0)
		{
			table[py][px] -= 1;
			if (table[py][px] == 0)
				table[py][px] = 4;
		}
	}
}


void CalculateBuildOrder(const std::vector< std::vector<int> >& buildings,
	std::vector< std::pair<size_t, size_t> >& solution)
{
	// dumb solution
	std::vector< std::vector<int> > table(buildings);
	size_t H = table.size();
	size_t W = table[0].size();
	std::vector< std::pair<size_t, size_t> > tried_stack;
	//std::vector< std::pair<size_t, size_t> > ones;
	size_t yy = 0;
	size_t xx = 0;
	bool removed_one = false;
	bool was_building = false;
	while (true)
	{
		for (; yy < H; ++yy)
		{
			for (; xx < W; ++xx)
			{
				if (table[yy][xx]>0)
					was_building = true;
				if (table[yy][xx] == 1)
				{
					tried_stack.push_back(std::make_pair(yy, xx));
					RemoveOne(table, yy, xx);
					removed_one = true;
					yy = xx = 0;
					break;
				}
			}
			if (removed_one)
				break;
			xx = 0;
		}
		if(!was_building)
		{
			size_t N = tried_stack.size();
			solution.resize(N);
			for (size_t i = 0; i < N; ++i)
			{
				solution[i] = tried_stack[N - i - 1];
			}
			return;
		}
		if (!removed_one)
		{
			if (tried_stack.empty())
			{
				assert(false && "lofasz");
				return;
			}
			auto last_one = tried_stack.back();
			tried_stack.pop_back();
			yy = last_one.first;
			xx = last_one.second;
			PutOne(table, yy, xx);
			xx++;
		}
		else
		{
			removed_one = false;
			was_building = false;
		}
	}
}