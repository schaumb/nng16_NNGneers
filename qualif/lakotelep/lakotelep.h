#pragma once

#include <vector>

void CalculateBuildOrder(const std::vector< std::vector<int> >& buildings,
	std::vector< std::pair<size_t, size_t> >& solution);