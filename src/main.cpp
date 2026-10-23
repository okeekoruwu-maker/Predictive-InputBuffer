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
    struct Fields {
        std::deque<BufferedInput> inputQueue;
        int frameCounter = 0;
        bool replayingInput = false;
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        auto* fields = m_fields.self();

        if (fields->replayingInput ||
            !Mod::get()->getSettingValue<bool>("enabled")) {
            GJBaseGameLayer::handleButton(
                down, button, isPlayer1
            );
            return;
        }

        int delay = static_cast<int>(
            Mod::get()->getSettingValue<int64_t>("buffer-frames")
        );

        if (delay <= 0) {
            GJBaseGameLayer::handleButton(
                down, button, isPlayer1
            );
            return;
        }

        fields->inputQueue.push_back({
            down,
            button,
            isPlayer1,
            fields->frameCounter + delay
        });
    }

    void update(float dt) {
        GJBaseGameLayer::update(dt);

        auto* fields = m_fields.self();
        fields->frameCounter++;

        while (!fields->inputQueue.empty() &&
               fields->inputQueue.front().targetFrame <= fields->frameCounter) {
            auto input = fields->inputQueue.front();
            fields->inputQueue.pop_front();

            fields->replayingInput = true;
            GJBaseGameLayer::handleButton(
                input.down,
                input.button,
                input.player1
            );
            fields->replayingInput = false;
        }
    }
};
