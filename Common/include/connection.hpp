#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <vector>
#include <iostream>

#include "yojimbo.h"

#define SERVER_PORT 4545
#define TICK_RATE 60

static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };

// a simple test message
enum class MessageType {
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
    std::vector<std::vector<uint32_t>> grid;
    uint64_t client_id;

    GridMessage(){}; 

    template <typename Stream> 
    bool Serialize( Stream & stream )
    {        
        // Serialze owner
        serialize_uint64(stream, client_id);
        // Serialze dimensions
        uint32_t num_rows = static_cast<uint32_t>(grid.size());
        serialize_uint32(stream, num_rows);
        
        uint32_t num_columns = 0;
        if(!grid.empty()){
            num_columns = static_cast<uint32_t>(grid[0].size());
        }

        serialize_uint32(stream, num_columns);

        // Serialze vector
        grid.resize(num_rows);
        for(uint32_t i = 0; i < num_rows; i++){
            grid[i].resize(num_columns);
            for(uint32_t j = 0; j<num_columns; j++){
                serialize_uint32(stream, grid[i][j]);
            }
        }

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};


// the message factory
YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)MessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::GRID, GridMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();


// the adapter
struct ClientAdapter : public yojimbo::Adapter {
    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override {
        return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
    }
};
#endif
