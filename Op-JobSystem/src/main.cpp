#include <iostream>


#include <JobSystem.h>
#include <chrono>
#include <string>


struct DummyStruct
{
	int dummyVar = 0;
	bool dummyVar2 = false;
};

void Spin(float milliseconds, DummyStruct& st = DummyStruct())
{
	milliseconds /= 1000.0f;
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	double ms = 0;
	while (ms < milliseconds)
	{
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		ms = time_span.count();
	}

	st.dummyVar = 100;
}

struct timer
{
	std::string name;
	std::chrono::high_resolution_clock::time_point start;

	timer(const std::string& name) : name(name), start(std::chrono::high_resolution_clock::now()) {}
	~timer()
	{
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << name << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds" << std::endl;
	}
};

int main(char** argv, int argc)
{

	OP::JobSystem::Initialize();

	DummyStruct st;

	// Serial test
	{
		auto t = timer("Serial test: ");
		Spin(1000);
		Spin(1000);
		Spin(1000);
		Spin(1000);
		Spin(1000);
		Spin(1000);
	}

	// Execute test
	{
		auto t = timer("Execute() test:");

		OP::JobTracker tracker;

		OP::JobSystem::Execute([&] { Spin(5000, st); }, tracker);

		std::cout << "First Function submitted to queue\n";

		OP::JobSystem::Execute([] { Spin(10000); });
		OP::JobSystem::Execute([] { Spin(10000); });
		OP::JobSystem::Execute([] { Spin(10000); });
		OP::JobSystem::Execute([] { Spin(10000); });
		OP::JobSystem::Execute([] { Spin(10000); });

		std::cout << "While loop starts\n";
		while (true)
		{
			if (tracker.IsFinished())
			{
				std::cout << "First Function has been executed\n";
				std::cout << st.dummyVar << "\n";
				break;
			}
		}

		OP::JobSystem::Wait();

		std::cout << "Other Functions have been executed\n";
	}


	return 0;
}