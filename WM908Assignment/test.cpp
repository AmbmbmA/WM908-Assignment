#include "GamesEngineeringBase.h"
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace GamesEngineeringBase;

struct Block
{
    int x, y, width, height;
    float r, g, b;
    int dx, dy;
};

int main()
{
    // Create a window instance
    Window window;
    window.create(1000, 1000, "aa");


    // Initialize game variables
    Block player = { 100, 100, 20, 20, 0.0f, 1.0f, 0.0f, 0, 0 }; // 20x20 player block, green color
    int playerSpeed = 10;

    std::vector<Block> blocks;
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < 5; ++i)
    {
        Block block;
        block.x = rand() % 700 + 50;
        block.y = rand() % 500 + 50;
        block.width = 20;
        block.height = 20;
        block.r = static_cast<float>(rand()) / RAND_MAX;
        block.g = static_cast<float>(rand()) / RAND_MAX;
        block.b = static_cast<float>(rand()) / RAND_MAX;
        block.dx = (rand() % 2 == 0) ? 5 : -5;
        block.dy = (rand() % 2 == 0) ? 5 : -5;
        blocks.push_back(block);
    }

    // Main game loop
    MSG msg = {};
    while (true)
    {
        // Check for messages
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                return 0;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        window.clear(); // Clear with black background

        // Handle keyboard input to move the player block
        if (GetAsyncKeyState(VK_UP) & 0x8000)
        {
            player.y -= playerSpeed;
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x8000)
        {
            player.y += playerSpeed;
        }
        if (GetAsyncKeyState(VK_LEFT) & 0x8000)
        {
            player.x -= playerSpeed;
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
        {
            player.x += playerSpeed;
        }

        // Update enemy blocks positions
        for (auto& block : blocks)
        {
            block.x += block.dx;
            block.y += block.dy;

            // Bounce off the walls
            if (block.x <= 0 || block.x + block.width >= 800)
            {
                block.dx = -block.dx;
            }
            if (block.y <= 0 || block.y + block.height >= 600)
            {
                block.dy = -block.dy;
            }
        }

        // Draw the player block pixel by pixel
        for (int i = 0; i < player.width; ++i)
        {
            for (int j = 0; j < player.height; ++j)
            {
                window.draw(player.x + i, player.y + j, player.r, player.g, player.b);
            }
        }

        // Draw the enemy blocks pixel by pixel
        for (const auto& block : blocks)
        {
            for (int i = 0; i < block.width; ++i)
            {
                for (int j = 0; j < block.height; ++j)
                {
                    window.draw(block.x + i, block.y + j, block.r, block.g, block.b);
                }
            }
        }

        // Present the frame
        window.present();
    }

    return 0;
}
