#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <deque>

using namespace geode;

struct BufferedInput {
    bool down;
    int button;
    bool player1;
    int targetFrame;
};

class $modify(InputBufferLayer, GJBaseGameLayer) {
    std::deque<BufferedInput> inputQueue;
    int frameCounter = 0;
    bool replayingInput = false;

    void handleButton(bool down, int button, bool isPlayer1) {
        if (replayingInput ||
            !Mod::get()->getSettingValue<bool>("enabled")) {
            GJBaseGameLayer::handleButton(
                down, button, isPlayer1
            );
            return;
        }

        int delay = Mod::get()->getSettingValue<int64_t>(
            "buffer-frames"
        );

        if (delay <= 0) {
            GJBaseGameLayer::handleButton(
                down, button, isPlayer1
            );
            return;
        }

        inputQueue.push_back({
            down,
            button,
            isPlayer1,
            frameCounter + delay
        });
    }

    void update(float dt) {
        GJBaseGameLayer::update(dt);
        frameCounter++;

        while (!inputQueue.empty() &&
               inputQueue.front().targetFrame <= frameCounter) {
            auto input = inputQueue.front();
            inputQueue.pop_front();

            replayingInput = true;
            GJBaseGameLayer::handleButton(
                input.down,
                input.button,
                input.player1
            );
            replayingInput = false;
        }
    }
};
