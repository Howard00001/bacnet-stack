#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <bacnet/bacenum.h>
#include <bacnet/rp.h>
#include <bacnet/wp.h>

// Define the socket path
#define SOCKET_PATH "/tmp/bacnet.socket"

//functions
const char* get_property_id_string(BACNET_PROPERTY_ID id);
char *processSocketOut(char *output);
bool check_object_type(BACNET_OBJECT_TYPE object_type);
void parse_property_string(BACNET_APPLICATION_DATA_VALUE *value, char *buffer);
void sync_read(BACNET_READ_PROPERTY_DATA *rpdata);
void sync_write(BACNET_WRITE_PROPERTY_DATA *wpdata);
char *write_to_socket(const char* message);
char *generate_str(const char* cmd, BACNET_OBJECT_TYPE object, BACNET_PROPERTY_ID property, char* value);