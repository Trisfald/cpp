#include "Chess_program.hpp"

#include <iostream>

int main()
{
	Chess_program game(read_team(std::cout, std::cin));
	while (game.result() == Result::none && !game.exit())
	{
		std::cout << game << std::endl;
		game.do_turn();
	}
	if (game.exit())
	{
		std::cout << "Bye" << std::endl;
	}
	else
	{
		std::cout << "Game finished! Result: " << game.result() << std::endl;
	}
    return 0;
}
