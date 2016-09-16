#include "Sudoku_board.hpp"
#include "Backtracking.hpp"
#include <iostream>
#include <chrono>
#include "Sudoku_csp.hpp"

Sudoku_values init_config_easy();
Sudoku_values init_config_intermediate();
Sudoku_values init_config_hard();
void test(const Sudoku_values&);

Backtracking_solver<> solver;

int main()
{
	std::cout << "Solving easy sudoku..." << std::endl;
	test(init_config_easy());
	std::cout << "Solving intermediate sudoku..." << std::endl;
	test(init_config_intermediate());
	std::cout << "Solving hard sudoku..." << std::endl;
	test(init_config_hard());
}

void test(const Sudoku_values& values)
{
	Sudoku_csp csp = create_csp(values);
	auto start = std::chrono::steady_clock::now();
	auto result = solver(csp);
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
	std::cout << "Time: " << duration.count() << "ms\n";
    if (result == nullptr)
    {
    	std::cout << "No result found!" << std::endl;
    }
    else
    {
    	std::cout << make_board(*result);
    }
    std::cout << "\n\n";
}

Sudoku_values init_config_easy()
{
	Sudoku_values ret;

	ret.insert({{0, 2}, 6});
	ret.insert({{0, 3}, 1});
	ret.insert({{0, 5}, 8});
	ret.insert({{0, 6}, 2});
	ret.insert({{0, 7}, 7});

	ret.insert({{1, 0}, 2});
	ret.insert({{1, 1}, 1});
	ret.insert({{1, 2}, 8});
	ret.insert({{1, 6}, 4});
	ret.insert({{1, 7}, 9});

	ret.insert({{2, 1}, 7});
	ret.insert({{2, 3}, 9});
	ret.insert({{2, 4}, 4});
	ret.insert({{2, 5}, 2});
	ret.insert({{2, 7}, 1});

	ret.insert({{3, 2}, 7});
	ret.insert({{3, 3}, 6});
	ret.insert({{3, 4}, 9});
	ret.insert({{3, 5}, 1});
	ret.insert({{3, 8}, 2});

	ret.insert({{4, 2}, 2});
	ret.insert({{4, 5}, 7});
	ret.insert({{4, 6}, 1});
	ret.insert({{4, 7}, 3});
	ret.insert({{4, 8}, 9});

	ret.insert({{5, 0}, 1});
	ret.insert({{5, 1}, 9});
	ret.insert({{5, 2}, 5});
	ret.insert({{5, 3}, 8});
	ret.insert({{5, 8}, 4});

	ret.insert({{6, 0}, 6});
	ret.insert({{6, 1}, 2});
	ret.insert({{6, 4}, 1});
	ret.insert({{6, 6}, 8});
	ret.insert({{6, 8}, 7});

	ret.insert({{7, 0}, 7});
	ret.insert({{7, 4}, 8});
	ret.insert({{7, 5}, 9});
	ret.insert({{7, 6}, 3});
	ret.insert({{7, 7}, 4});

	ret.insert({{8, 0}, 3});
	ret.insert({{8, 1}, 8});
	ret.insert({{8, 3}, 7});
	ret.insert({{8, 4}, 6});
	ret.insert({{8, 8}, 1});

	return ret;
}

Sudoku_values init_config_intermediate()
{
	Sudoku_values ret;

	ret.insert({{0, 3}, 3});
	ret.insert({{0, 6}, 4});

	ret.insert({{1, 0}, 9});
	ret.insert({{1, 4}, 1});

	ret.insert({{2, 0}, 4});
	ret.insert({{2, 2}, 8});
	ret.insert({{2, 4}, 7});
	ret.insert({{2, 6}, 5});
	ret.insert({{2, 8}, 3});

	ret.insert({{3, 3}, 6});
	ret.insert({{3, 6}, 9});

	ret.insert({{4, 3}, 1});
	ret.insert({{4, 4}, 5});
	ret.insert({{4, 5}, 7});

	ret.insert({{5, 2}, 5});
	ret.insert({{5, 5}, 2});

	ret.insert({{6, 0}, 2});
	ret.insert({{6, 2}, 4});
	ret.insert({{6, 6}, 6});
	ret.insert({{6, 8}, 5});

	ret.insert({{7, 4}, 2});
	ret.insert({{7, 8}, 8});

	ret.insert({{8, 2}, 6});
	ret.insert({{8, 3}, 7});
	ret.insert({{8, 5}, 8});

	return ret;
}

Sudoku_values init_config_hard()
{
	Sudoku_values ret;

	ret.insert({{0, 2}, 6});
	ret.insert({{0, 7}, 7});
	ret.insert({{0, 8}, 5});

	ret.insert({{1, 0}, 3});
	ret.insert({{1, 4}, 2});
	ret.insert({{1, 8}, 1});

	ret.insert({{2, 0}, 5});
	ret.insert({{2, 1}, 7});
	ret.insert({{2, 3}, 3});

	ret.insert({{3, 4}, 3});
	ret.insert({{3, 7}, 9});
	ret.insert({{3, 8}, 8});

	ret.insert({{4, 8}, 4});

	ret.insert({{5, 1}, 8});
	ret.insert({{5, 2}, 4});
	ret.insert({{5, 6}, 6});

	ret.insert({{6, 1}, 6});
	ret.insert({{6, 4}, 4});
	ret.insert({{6, 6}, 5});

	ret.insert({{7, 0}, 9});
	ret.insert({{7, 5}, 5});
	ret.insert({{7, 6}, 1});

	ret.insert({{8, 3}, 7});
	ret.insert({{8, 5}, 8});

	return ret;
}

