// lakotelep.cpp : Defines the entry point for the console application.
//

#include "lakotelep.h"
#include <iostream>
#include "stdio.h"
#include <algorithm>
#include <random>
#include <cassert>

bool BuildSolution(const std::vector< std::pair<size_t, size_t> >& solution, std::vector< std::vector<int> >& output)
{
	const size_t H = output.size();
	const size_t W = output[0].size();
	if (solution.size() < H*W)
		return false;
	std::vector< std::vector<int> > tmp;
	tmp.resize(H);
	for (size_t i = 0; i < H; ++i)
	{
		tmp[i].resize(W);
	}
	for (std::pair<size_t, size_t> it : solution)
	{
		if (it.first >= H)
			return false;
		if (it.second >= W)
			return false;
		if (tmp[it.first][it.second] > 0)
			return false;
		tmp[it.first][it.second] = 1;
		// H boundary
		if (it.first < H - 1)
		{
			if (tmp[it.first + 1][it.second] > 0)
			{
				tmp[it.first + 1][it.second] += 1;
				if (tmp[it.first + 1][it.second] == 5)
					tmp[it.first + 1][it.second] = 1;
			}
		}
		if (it.first > 0)
		{
			if (tmp[it.first - 1][it.second] > 0)
			{
				tmp[it.first - 1][it.second] += 1;
				if (tmp[it.first - 1][it.second] == 5)
					tmp[it.first - 1][it.second] = 1;
			}
		}
		// W boundary
		if (it.second < W - 1)
		{
			if (tmp[it.first][it.second + 1] > 0)
			{
				tmp[it.first][it.second + 1] += 1;
				if (tmp[it.first][it.second + 1] == 5)
					tmp[it.first][it.second + 1] = 1;
			}
		}
		if (it.second > 0)
		{
			if (tmp[it.first][it.second - 1] > 0)
			{
				tmp[it.first][it.second - 1] += 1;
				if (tmp[it.first][it.second - 1] == 5)
					tmp[it.first][it.second - 1] = 1;
			}
		}
	}
	tmp.swap(output);
	return true;
}

bool CheckSolution(const std::vector< std::pair<size_t, size_t> >& solution, const std::vector< std::vector<int> >& original)
{
	std::vector< std::vector<int> > attempt;
	attempt.resize(original.size());
	attempt[0].resize(original[0].size());
	if (BuildSolution(solution, attempt))
	{
		return original == attempt;
	}
	return false;
}

int main()
{
	//test1:
	std::vector < std::vector<int> > test;
	test.resize(2);
	test[0].resize(2);
	test[1].resize(2);
	test[0][0] = 2;
	test[1][0] = 3;
	test[1][1] = 2;
	test[0][1] = 1;
	std::vector< std::pair<size_t, size_t> > solution;
	CalculateBuildOrder(test, solution);
	bool succ = CheckSolution(solution, test);
	std::cout << "handmade test 0 " << (succ?"success":"failed") << std::endl;
	std::mt19937 generator(1337);
	std::vector< std::pair<size_t, size_t> > orig_plan;
	size_t OK_count = 0;
	size_t FAIL_count = 0;
	for (size_t testcount = 0; testcount < 1000; ++testcount)
	{
		size_t h = /*generator() % 99 + 1*/4;
		size_t w = /*generator() % 99 + 1*/4;
		orig_plan.resize(0);
		orig_plan.reserve(h*w);
		test.resize(h);
		test[0].resize(w);
		for (size_t i = 0; i < h; ++i)
		{
			for (size_t j = 0; j < w; ++j)
			{
				orig_plan.push_back(std::make_pair(i, j));
			}
		}
		std::random_shuffle(orig_plan.begin(), orig_plan.end());
		bool siker = BuildSolution(orig_plan, test);
		assert(siker);
		std::cout << "--------------------------------------------------------------------------------------------------------" << std::endl;
		for (auto row : test)
		{
			for (auto cell : row)
				std::cout << cell;
			std::cout << std::endl;
		}
		solution.clear();
		CalculateBuildOrder(test, solution);
		bool succ = CheckSolution(solution, test);
		if (succ)
			OK_count++;
		else
			FAIL_count++;
		std::cout << "test" << testcount << " " << (succ ? "success" : "failed") << std::endl;
	}
	std::cout << "test finished, " << OK_count << " succeeded, " << FAIL_count << " failed" << std::endl;
	getchar();
    return 0;
}
