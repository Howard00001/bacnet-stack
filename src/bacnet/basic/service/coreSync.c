#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <bacnet/basic/service/coreSync.h>
#include <bacnet/bacenum.h>
#include <bacnet/rp.h>
#include <bacnet/wp.h>
#include <bacnet/basic/object/device.h>

#define NUM_DEVICE_PROPERTY 38
const char* get_property_id_string(BACNET_PROPERTY_ID id) {
    switch (id) {
        case PROP_OBJECT_IDENTIFIER: return "OBJECT_IDENTIFIER";
        case PROP_OBJECT_NAME: return "OBJECT_NAME";
        case PROP_OBJECT_TYPE: return "OBJECT_TYPE";
        case PROP_SYSTEM_STATUS: return "SYSTEM_STATUS";
        case PROP_VENDOR_NAME: return "VENDOR_NAME";
        case PROP_VENDOR_IDENTIFIER: return "VENDOR_IDENTIFIER";
        case PROP_MODEL_NAME: return "MODEL_NAME";
        case PROP_FIRMWARE_REVISION: return "FIRMWARE_REVISION";
        case PROP_APPLICATION_SOFTWARE_VERSION: return "APPLICATION_SOFTWARE_VERSION";
        case PROP_PROTOCOL_VERSION: return "PROTOCOL_VERSION";
        case PROP_PROTOCOL_REVISION: return "PROTOCOL_REVISION";
        case PROP_PROTOCOL_SERVICES_SUPPORTED: return "PROTOCOL_SERVICES_SUPPORTED";
        case PROP_PROTOCOL_OBJECT_TYPES_SUPPORTED: return "PROTOCOL_OBJECT_TYPES_SUPPORTED";
        case PROP_MAX_APDU_LENGTH_ACCEPTED: return "MAX_APDU_LENGTH_ACCEPTED";
        case PROP_SEGMENTATION_SUPPORTED: return "SEGMENTATION_SUPPORTED";
        case PROP_APDU_TIMEOUT: return "APDU_TIMEOUT";
        case PROP_NUMBER_OF_APDU_RETRIES: return "NUMBER_OF_APDU_RETRIES";
        case PROP_DEVICE_ADDRESS_BINDING: return "DEVICE_ADDRESS_BINDING";
        case PROP_DATABASE_REVISION: return "DATABASE_REVISION";
        case PROP_DESCRIPTION: return "DESCRIPTION";
        case PROP_LOCAL_TIME: return "LOCAL_TIME";
        case PROP_DAYLIGHT_SAVINGS_STATUS: return "DAYLIGHT_SAVINGS_STATUS";
        case PROP_LOCATION: return "LOCATION";
        case PROP_ACTIVE_COV_SUBSCRIPTIONS: return "ACTIVE_COV_SUBSCRIPTIONS";
        case PROP_TIME_SYNCHRONIZATION_RECIPIENTS: return "TIME_SYNCHRONIZATION_RECIPIENTS";
        case PROP_TIME_SYNCHRONIZATION_INTERVAL: return "TIME_SYNCHRONIZATION_INTERVAL";
        case PROP_ALIGN_INTERVALS: return "ALIGN_INTERVALS";
        case PROP_INTERVAL_OFFSET: return "INTERVAL_OFFSET";
        default: return "";
    }
}

/* 
valid service_choice: 
    SERVICE_CONFIRMED_READ_PROPERTY
    SERVICE_CONFIRMED_READ_PROP_MULTIPLE
    SERVICE_CONFIRMED_WRITE_PROPERTY
    SERVICE_CONFIRMED_WRITE_PROP_MULTIPLE

valid object_type:
    OBJECT_DEVICE
*/

char *processSocketOut(char *output) {
    // TODO: handle convert output string
    return output;
}

void sync_read(BACNET_READ_PROPERTY_DATA *rpdata) {
    if (rpdata->object_type != OBJECT_DEVICE) {
        return;
    }
    char *msg = generate_str("READ", rpdata->object_type, rpdata->object_property, NULL);
    printf("msg: %s\n", msg);
    if (msg == "") {
        return;
    }
    char *output = write_to_socket(msg);
    BACNET_CHARACTER_STRING char_string;
    if (strlen(output) != 0){
        printf("output: %s\n", output);
        // write to bacnet stack
        BACNET_WRITE_PROPERTY_DATA wp_data = {
            .object_type = rpdata->object_type,
            .object_instance = rpdata->object_instance,
            .object_property = rpdata->object_property,
            .priority = BACNET_NO_PRIORITY,
            .array_index = rpdata->array_index,
            .error_class = rpdata->error_class,
            .error_code = rpdata->error_code
            //.application_data //uint8_t[]
        };

        char *ret = processSocketOut(output);
        characterstring_init_ansi(&char_string, ret);
        encode_bacnet_character_string(&wp_data.application_data, &char_string);
        encode_application_character_string(&wp_data.application_data, &char_string);
        uint32_t i;
        size_t length = characterstring_length(&char_string)+3;
        wp_data.application_data_len = length;

        Device_Write_Property(&wp_data);
    }
}

char *write_to_socket(const char* message) {
    int sockfd;
    struct sockaddr_un serv_addr;

    // create socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    // init socket server struct
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SOCKET_PATH);

    // connect to server
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to socket");
        exit(1);
    }

    // write to socket
    write(sockfd, message, strlen(message));

    // read from socket
    char *buffer = malloc(512); 
    int n = read(sockfd, buffer, 512);
    if (n < 0) {
        perror("Error reading from socket");
        free(buffer);
        exit(1);
    }
    buffer[n] = '\0';

    // close socket connection
    close(sockfd);

    return buffer;
}

char *generate_str(const char* cmd, BACNET_OBJECT_TYPE object, BACNET_PROPERTY_ID property, char* value) {
    // return : {READ/WRITE}$SYSTEM${attribute}${value}
    
    const char* objectStr;
    switch (object) {
        case OBJECT_DEVICE:
            objectStr = "DEVICE";
            break;
        default:
            return "";
    }
    const char* idStr = get_property_id_string(property);
    if(idStr == "") {
        return "";
    }
    char *result;
    if(value == NULL) {
        int length = snprintf(NULL, 0, "%s$%s$%s", cmd, objectStr, idStr);
        result = (char*)malloc(length + 1);
        snprintf(result, length + 1, "%s$%s$%s", cmd, objectStr, idStr);
    } else {
        int length = snprintf(NULL, 0, "%s$%s$%s$%s", cmd, objectStr, idStr, value);
        result = (char*)malloc(length + 1);
        snprintf(result, length + 1, "%s$%s$%s$%s", cmd, objectStr, idStr, value);
    }
    return result;
}