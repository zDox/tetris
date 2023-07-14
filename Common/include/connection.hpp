#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "yojimbo.h"

#define SERVER_PORT 4545

static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };

// a simple test message
enum class GameMessageType {
    GRID,
    COUNT
};

// two channels, one for each type that Yojimbo supports
enum class GameChannel {
    RELIABLE,
    UNRELIABLE,
    COUNT
};

// the client and server config
struct GameConnectionConfig : yojimbo::ClientServerConfig {
    GameConnectionConfig()  {
        numChannels = 1;
        channel[(int)GameChannel::RELIABLE].type = yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED;
        protocolId = 0x0000000000000001;
    }
};

static GameConnectionConfig game_connection_config;

struct GridMessage : public yojimbo::Message
{
    uint32_t sequence[20*10];

    GridMessage()
    {
        for(int i=0; i < 20*10; i++){
            sequence[i] = 0x0;
        }
    }

    template <typename Stream> 
    bool Serialize( Stream & stream )
    {        
        for(int i=0; i <20*10; i++){
            serialize_bits(stream, sequence[i], 32 );
        }
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};


// the message factory
YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)GameMessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::GRID, GridMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();


// the adapter
struct GameAdapter : public yojimbo::Adapter {
    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override {
        return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
    }
};
static GameAdapter adapter;
#endif
