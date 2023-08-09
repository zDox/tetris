#ifndef ROUNDSTATE_TYPE
#define ROUNDSTATE_TYPE
#include <string>

enum class RoundStateType {
    LOBBY,
    INGAME,
    END,
    DEAD,
    COUNT
};

namespace std{
    inline string to_string(RoundStateType type){
        switch (type){
            case RoundStateType::LOBBY:
                return "Lobby";
            case RoundStateType::INGAME:
                return "Ingame";
            case RoundStateType::END:
                return "Ending";
            default:
                return "Unknown";
        }
    }
}
#endif
