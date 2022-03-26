#include <iostream>


#include <JobSystem.h>

struct DummyGame
{
	void Init()
	{
		std::cout << "Game has been initialized!\n";
	}

	void Run()
	{

	}
private:
};

int main(char** argv, int argc)
{
	bool running = true;
	OP::JobSystem jobSystem{};

	jobSystem.Initialize();

	while (running)
	{
		OP::Command command{0};

		std::cout << "Give command! (must be an integer)\n";
		std::cin >> command;

		switch (command)
		{
			case 0:
				running = false;
				break;

			default:
				jobSystem.QueueCommand(command);
		}
	}

	std::cout << "GameLoop Ended\n";


	return 0;
}