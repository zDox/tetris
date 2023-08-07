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
    GAME_DATA,
    TETRAMINO_PLACEMENT,
    PLAYER_CONNECT_REQUEST, // Player Message when trying connecting to GameServer
    PLAYER_CONNECT_RESPONSE, // Response Message to Connection attempt from client
    PLAYER_DATA,
    PLAYER_JOIN, // Message when connecting to Game
    PLAYER_LEAVE, // Message when disconnecting from Game 
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
        protocolId = 4;
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

struct GameDataMessage : public yojimbo::Message
{
    std::uint64_t bytes;
    int game_id;
    RoundStateType roundstate;
    int min_players;
    int max_players;
    std::vector<uint64_t> players;

    GameDataMessage(){}; 

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
                in(roundstate).or_throw();
                in(min_players).or_throw();
                in(max_players).or_throw();
                in(players).or_throw();
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
                out(roundstate).or_throw();
                out(min_players).or_throw();
                out(max_players).or_throw();
                out(players).or_throw();
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

struct PlayerConnectRequestMessage : public yojimbo::Message
{
    std::uint64_t bytes;
    std::string name;

    PlayerConnectRequestMessage(){}; 

    template <typename Stream> 
    bool Serialize( Stream & stream )
    {        
        auto [data, in, out] = zpp::bits::data_in_out();
        if(Stream::IsReading){
            try{
                serialize_uint64(stream, bytes);
                data.resize(bytes);
                serialize_bytes(stream, reinterpret_cast<uint8_t*>(data.data()), bytes);
                in(name).or_throw();
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
                out(name).or_throw();
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

struct PlayerConnectResponseMessage : public yojimbo::Message
{
    PlayerConnectResult result;
    
    PlayerConnectResponseMessage(){}; 

    template <typename Stream> 
    bool Serialize( Stream & stream ){        
        int result_value = static_cast<int>(result);
        serialize_int(stream, result_value, 0, 64);
        result = static_cast<PlayerConnectResult>(result_value);  
        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

struct PlayerDataMessage : public yojimbo::Message
{
    std::uint64_t bytes;
    Player player;

    PlayerDataMessage(){}; 

    template <typename Stream> 
    bool Serialize( Stream & stream )
    {        
        auto [data, in, out] = zpp::bits::data_in_out();
        if(Stream::IsReading){
            try{
                serialize_uint64(stream, bytes);
                data.resize(bytes);
                serialize_bytes(stream, reinterpret_cast<uint8_t*>(data.data()), bytes);
                in(player).or_throw();
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
                out(player).or_throw();
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
    PlayerInput player_input; 

    PlayerInputMessage(){}; 

    template <typename Stream> 
    bool Serialize( Stream & stream ){ 
        serialize_int(stream, game_id, -1, std::numeric_limits<int>::max());
        serialize_int(stream, player_input.frame, -1, std::numeric_limits<int>::max());
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
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::GAME_DATA, GameDataMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::TETRAMINO_PLACEMENT, TetraminoPlacementMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::PLAYER_CONNECT_REQUEST, PlayerConnectRequestMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::PLAYER_CONNECT_RESPONSE, PlayerConnectResponseMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::PLAYER_DATA, PlayerDataMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::PLAYER_JOIN, PlayerJoinMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::PLAYER_LEAVE, PlayerLeaveMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE((int)MessageType::PLAYER_INPUT, PlayerInputMessage);
YOJIMBO_MESSAGE_FACTORY_FINISH();


#define GENERATE_MESSAGE_TYPE_CASE(Type) \
    case static_cast<int>(MessageType::Type): \
        return MessageType::Type;

inline MessageType convToMessageType(int message_type){
    switch(message_type){
        GENERATE_MESSAGE_TYPE_CASE(GRID)
        GENERATE_MESSAGE_TYPE_CASE(GAME_DATA)
        GENERATE_MESSAGE_TYPE_CASE(TETRAMINO_PLACEMENT)
        GENERATE_MESSAGE_TYPE_CASE(PLAYER_CONNECT_REQUEST)
        GENERATE_MESSAGE_TYPE_CASE(PLAYER_CONNECT_RESPONSE)
        GENERATE_MESSAGE_TYPE_CASE(PLAYER_DATA)
        GENERATE_MESSAGE_TYPE_CASE(PLAYER_JOIN)
        GENERATE_MESSAGE_TYPE_CASE(PLAYER_LEAVE)
        GENERATE_MESSAGE_TYPE_CASE(PLAYER_INPUT)
        default:
            return MessageType::COUNT;
    }
}

// the adapter
struct ClientAdapter : public yojimbo::Adapter {
    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override {
        return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
    }
};
#endif
