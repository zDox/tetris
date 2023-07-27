#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <vector>
#include <iostream>
#include <limits>

#include "yojimbo.h"
#include "zpp_bits.h"

#include "TetraminoType.hpp"
#include "RoundStateType.hpp"
#include "Player.hpp"

#define SERVER_PORT 4545
#define TICK_RATE 60

static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };

// a simple test message
enum class MessageType {
    GRID,
    ROUNDSTATECHANGE,
    TETRAMINO_PLACEMENT,
    PLAYER_SCORE,
    PLAYER_JOIN,
    PLAYER_LEAVE,
    PLAYER_INPUT,
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
        protocolId = 3;
    }
};

struct GridMessage : public yojimbo::Message
{
    int game_id;
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
                in(game_id).or_throw();
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
                out(game_id).or_throw();
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

struct RoundStateChangeMessage : public yojimbo::Message
{
    int game_id;
    RoundStateType roundstate;
    RoundStateChangeMessage(){
        roundstate = RoundStateType::LOBBY;
    }; 

    template <typename Stream> 
    bool Serialize( Stream & stream ){        
        serialize_int(stream, game_id, 0, std::numeric_limits<int>::max());
        int roundstate_value = static_cast<int>(roundstate);
        serialize_int(stream, roundstate_value, 0, 64);
        roundstate = static_cast<RoundStateType>(roundstate_value);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

struct TetraminoPlacementMessage : public yojimbo::Message
{
    int game_id;
    TetraminoType tetramino_type;


    TetraminoPlacementMessage(){}; 

    template <typename Stream> 
    bool Serialize( Stream & stream ){        
        serialize_int(stream, game_id, 0, std::numeric_limits<int>::max());
        int tetramino_type_value = static_cast<int>(tetramino_type);
        serialize_int(stream, tetramino_type_value, 0, 32);
        tetramino_type = static_cast<TetraminoType>(tetramino_type_value);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

struct PlayerScoreMessage : public yojimbo::Message
{
    int game_id;
    uint64_t client_id;
    int position;
    int points;


    PlayerScoreMessage(){}; 

    template <typename Stream> 
    bool Serialize( Stream & stream ){        
        serialize_int(stream, game_id, 0, std::numeric_limits<int>::max());
        serialize_uint64(stream, client_id);
        serialize_int(stream, position, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        serialize_int(stream, points, 0, std::numeric_limits<int>::max());
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

struct PlayerJoinMessage : public yojimbo::Message
{
    int game_id;
    uint64_t client_id;
    
    PlayerJoinMessage(){}; 

    template <typename Stream> 
    bool Serialize( Stream & stream ){        
        serialize_int(stream, game_id, 0, std::numeric_limits<int>::max());
        serialize_uint64(stream, client_id);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

struct PlayerLeaveMessage : public yojimbo::Message
{
    int game_id;
    uint64_t client_id;
    
    PlayerLeaveMessage(){}; 

    template <typename Stream> 
    bool Serialize( Stream & stream ){        
        serialize_int(stream, game_id, 0, std::numeric_limits<int>::max());
        serialize_uint64(stream, client_id);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

struct PlayerInputMessage : public yojimbo::Message
{

    int game_id;
    int frame;
    PlayerInput player_input; 

    PlayerInputMessage(){}; 

    template <typename Stream> 
    bool Serialize( Stream & stream ){ 
        serialize_int(stream, frame, 0, std::numeric_limits<int>::max());
        serialize_bool(stream, player_input.left);
        serialize_bool(stream, player_input.right);
        serialize_bool(stream, player_input.up);
        serialize_bool(stream, player_input.down);
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

// the message factory
YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)MessageType::COUNT);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::GRID, GridMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::ROUNDSTATECHANGE, RoundStateChangeMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::TETRAMINO_PLACEMENT, TetraminoPlacementMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::PLAYER_SCORE, PlayerScoreMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::PLAYER_JOIN, PlayerJoinMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::PLAYER_LEAVE, PlayerLeaveMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::PLAYER_INPUT, PlayerInputMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();


// the adapter
struct ClientAdapter : public yojimbo::Adapter {
    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override {
        return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
    }
};
#endif
