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
#define SOCKET_PATH "/tmp/tmp.socket"

//functions
char *processSocketOut(char *output);
void sync_read(BACNET_READ_PROPERTY_DATA *rpdata);
char *write_to_socket(const char* message);
char *generate_str(const char* cmd, BACNET_OBJECT_TYPE object, BACNET_PROPERTY_ID property, char* value);
// // void reverseBinaryString(char *str);
// // CipShortString convertToCipShortString(const char* str);
// int parse_request(CipMessageRouterRequest *const message_router_request);
// // void sync_identity(uint16_t cmd, uint16_t attribute);
// // void sync_identity_all(uint16_t cmd);
// // void parse_sync_ethernetlink(char *output, uint16_t interface, uint16_t attribute);
// // void sync_ethernetlink(uint16_t cmd, uint16_t interface, uint16_t attribute, const CipOctet *data);
// // void sync_ethernetlink_all(uint16_t cmd, uint16_t interface);
// // char *write_to_socket(const char* message);
// // char *generate_ethernetlink_str(uint16_t cmd, uint16_t interface, char* attribute, char* value);
// // char *generate_identity_str(uint16_t cmd, char* attribute, char* value);