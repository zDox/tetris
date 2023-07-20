#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <vector>
#include <iostream>

#include "yojimbo.h"
#include "zpp_bits.h"

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
    std::vector<std::vector<std::uint32_t>> grid;
    std::uint64_t client_id;
    std::uint64_t bytes;

    GridMessage(){}; 

    template <typename Stream> 
    bool Serialize( Stream & stream )
    {        
        auto [data, in, out] = zpp::bits::data_in_out();
        if(Stream::IsReading){
            try{
                serialize_uint64(stream, bytes);
                data.resize(bytes);
                serialize_bytes(stream, reinterpret_cast<uint8_t*>(data.data()), bytes);
                in(client_id).or_throw();
                in(grid).or_throw();

            }
            catch (const std::exception & error) {
                std::cout << "Failed reading with error: " << error.what() << '\n';
                return false;
            } catch (...) {
                std::cout << "Unknown error\n";
                return false;
            };
        }
        else {
            try{
                out(client_id).or_throw();
                out(grid).or_throw();
                bytes = data.size();
                serialize_uint64(stream, bytes);
                serialize_bytes(stream, reinterpret_cast<uint8_t*>(data.data()), bytes);

            }
            catch (const std::exception & error) {
                std::cout << "Failed writing with error: " << error.what() << '\n';
                return false;
            } catch (...) {
                std::cout << "Unknown error\n";
                return false;
            };
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
