#include "types.h"

typedef enum command
{
	INSERT,
	GET,
	EXIT,
	UPDATE,
	DELETE
} command_t;

typedef struct payload
{
    command_t command;
    uchar_t str[STR_SIZE];
    uint32_t num;
} payload_t;

uchar_t* serialize_payload(uchar_t *buffer, payload_t *value);
uchar_t* deserialize_payload(uchar_t *buffer, payload_t *value);
