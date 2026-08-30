#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "core/timer.h"
#include "buffer/buffer_manager.h"
#include "land/land_module.h"
#include "life/life_module.h"
#include "mind/mind_module.h"
#include "network/network_module.h"
#include "ui/ui_module.h"
#include "script/script_manager.h"
#include "python/python_bridge.h"
using namespace sdgs;
using namespace std::chrono_literals;
BufferManager bufferManager;
LandModule land;
LifeModule life;
MindModule mind;
NetworkModule network;
UIModule ui;
ScriptManager scriptManager;
PythonBridge pythonBridge;
std::atomic<bool> running{true};
std::jthread gameLoopThread;
void gameLoop()
{
	HighResTimer timer(60.0);
	timer.reset();
	while (running)
	{
		timer.wait();
		float dt = 1.0f / 60.0f;
		mind.update(dt);
		life.update(dt);
		land.update(dt);
		static int frameCounter = 0;
		if (++frameCounter % 10 == 0)
			pythonBridge.update();
		network.update();
		ui.update();
	}
}
int main(int argc, char* argv[])
{
	std::cout << "========================================" << std::endl;
	std::cout << "  SDGS Core v1.0 (Win10/11)" << std::endl;
	std::cout << "========================================" << std::endl;
	std::string worldPath = "worlds/default";
	if (argc > 1)
		worldPath = argv[1];
	bufferManager.setBaseDir("buffer");
	bufferManager.clearAll();
	std::ifstream configFile(worldPath + "/config/world.lg");
	nlohmann::json worldConfig;
	if (!configFile)
	{
		std::cerr << "Config not found!" << std::endl;
		return 1;
	}
	configFile >> worldConfig;
	land.init(worldPath, worldConfig);
	life.init(worldPath, worldConfig);
	mind.init(worldPath, worldConfig);
	network.init(worldPath);
	ui.init(worldPath);
	scriptManager.loadHandlers();
	scriptManager.loadAllScripts(worldPath + "/scripts");
	pythonBridge.init(worldPath);
	std::cout << "World initialized. Starting game loop..." << std::endl;
	gameLoopThread = std::jthread(gameLoop);
	std::cout << "Press Enter to stop..." << std::endl;
	std::cin.get();
	running = false;
	if (gameLoopThread.joinable())
		gameLoopThread.join();
	bufferManager.clearAll();
	std::cout << "Goodbye!" << std::endl;
	return 0;
}