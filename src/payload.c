#include "payload.h"

uchar_t* serialize_uint32(uchar_t *buffer, uint32_t value)
{
    buffer[0] = (value >> 24) & 0xFF;
    buffer[1] = (value >> 16) & 0xFF;
    buffer[2] = (value >> 8) & 0xFF;
    buffer[3] = value & 0xFF;
    return buffer + sizeof(uint32_t);
}

uchar_t* deserialize_uint32(uchar_t *buffer, uint32_t *value)
{
    (*value) = ( buffer[3]
                | ( (uint32_t)buffer[2] << 8 )
                | ( (uint32_t)buffer[1] << 16 )
                | ( (uint32_t)buffer[0] << 24 ) );
    return buffer + sizeof(uint32_t);
}

uchar_t* serialize_uchar(uchar_t *buffer, uchar_t value)
{
    buffer[0] = value;
    return buffer + sizeof(uchar_t);
}

uchar_t* deserialize_uchar(uchar_t *buffer, uchar_t *value)
{
    (*value) = buffer[0];
    return buffer + sizeof(uchar_t);
}

uchar_t* serialize_payload(uchar_t *buffer, payload_t *value)
{
    buffer = serialize_uint32(buffer, value->command);
    for(int32_t i = 0; i < STR_SIZE; i++)
    {
        buffer = serialize_uchar(buffer, value->str[i]);
    }
    buffer = serialize_uint32(buffer, value->num);
    return buffer;
}

uchar_t* deserialize_payload(uchar_t *buffer, payload_t *value)
{
    buffer = deserialize_uint32(buffer, &(value->command));
    for(int32_t i = 0; i < STR_SIZE; i++)
    {
        buffer = deserialize_uchar(buffer, &(value->str[i]));
    }
    buffer = deserialize_uint32(buffer, &(value->num));
    return buffer;
}
