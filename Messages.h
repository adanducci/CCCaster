#pragma once

#include "Constants.h"
#include "Protocol.h"
#include "Logger.h"

#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>

#include <array>


struct EndOfMessages : public SerializableSequence { EMPTY_MESSAGE_BOILERPLATE ( EndOfMessages ) };
struct CharaSelectLoaded : public SerializableSequence { EMPTY_MESSAGE_BOILERPLATE ( CharaSelectLoaded ) };


struct ClientType : public SerializableSequence
{
    ENUM_MESSAGE_BOILERPLATE ( ClientType, Host, Client )
};


struct NetplaySetup : public SerializableSequence
{
    uint8_t delay = 0;
    uint8_t training = 0;
    uint8_t hostPlayer = 0;

    NetplaySetup() {}

    PROTOCOL_BOILERPLATE ( delay, training, hostPlayer )
};


struct RngState : public SerializableSequence
{
    uint32_t rngState0, rngState1, rngState2;
    std::array<char, CC_RNGSTATE3_SIZE> rngState3;

    RngState() {}

    PROTOCOL_BOILERPLATE ( rngState0, rngState1, rngState2, rngState3 );
};


struct PlayerInputs : public SerializableMessage
{
    uint32_t frame = 0;
    uint16_t index = 0;

    // Represents the input range [frame - NUM_INPUTS + 1, frame + 1)
    std::array<uint16_t, NUM_INPUTS> inputs;

    uint32_t getStartFrame() const { return ( frame + 1 < NUM_INPUTS ) ? 0 : frame + 1 - NUM_INPUTS; }
    uint32_t getEndFrame() const { return frame + 1; }
    size_t size() const { return getEndFrame() - getStartFrame(); }

    PlayerInputs() {}
    PlayerInputs ( uint32_t frame, uint16_t index ) : frame ( frame ), index ( index ) {}

    PROTOCOL_BOILERPLATE ( frame, index, inputs )
};


class DoubleInputs : public SerializableMessage
{
    PlayerInputs inputs[2];

public:

    uint32_t getFrame() const { return inputs[0].frame; }
    void setFrame ( uint32_t frame ) { inputs[0].frame = inputs[1].frame = frame; }

    uint32_t getIndex() const { return inputs[0].frame; }
    void setIndex ( uint16_t index ) { inputs[0].index = inputs[1].index = index; }

    std::array<uint16_t, NUM_INPUTS>& getInputs ( uint8_t player )
    {
        ASSERT ( player == 1 || player == 2 );
        return inputs[player - 1].inputs;
    }

    const PlayerInputs& get ( uint8_t player ) const
    {
        ASSERT ( player == 1 || player == 2 );
        return inputs[player - 1];
    }

    DoubleInputs() {}
    DoubleInputs ( uint32_t frame, uint16_t index )
    {
        inputs[0].frame = inputs[1].frame = frame;
        inputs[0].index = inputs[1].index = index;
    }

    // Protocol methods

    MsgType getMsgType() const override;

    void save ( cereal::BinaryOutputArchive& ar ) const override
    {
        ar ( inputs[0].frame, inputs[0].index, inputs[0].inputs, inputs[1].inputs );
    }

    void load ( cereal::BinaryInputArchive& ar ) override
    {
        ar ( inputs[0].frame, inputs[0].index, inputs[0].inputs, inputs[1].inputs );
        inputs[1].frame = inputs[0].frame;
        inputs[1].index = inputs[0].index;
    }
};
