#pragma once

#include <vector>

void CalculateBuildOrder(const std::vector< std::vector<int> >& buildings,
	std::vector< std::pair<std::size_t, std::size_t> >& solution);
