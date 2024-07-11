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
        // Device Properties
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

        // Network Properties

        // Else
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

bool check_object_type(BACNET_OBJECT_TYPE object_type) {
    if (object_type == OBJECT_DEVICE) {
        return true;
    }
    return false;
}

void parse_property_string(BACNET_APPLICATION_DATA_VALUE *value, char *buffer) {
    size_t i;
    if (value) {
        switch (value->tag) {
            case BACNET_APPLICATION_TAG_NULL:
                break;
            case BACNET_APPLICATION_TAG_BOOLEAN:
                snprintf(buffer, sizeof(buffer), "%s", value->type.Boolean ? "true" : "false");
                break;
            case BACNET_APPLICATION_TAG_UNSIGNED_INT:
                snprintf(buffer, sizeof(buffer), "%u", value->type.Unsigned_Int);
                break;
            case BACNET_APPLICATION_TAG_SIGNED_INT:
                snprintf(buffer, sizeof(buffer), "%d", value->type.Signed_Int);
                break;
            case BACNET_APPLICATION_TAG_REAL:
                snprintf(buffer, sizeof(buffer), "%f", value->type.Real);
                break;
            case BACNET_APPLICATION_TAG_DOUBLE:
                snprintf(buffer, sizeof(buffer), "%f", value->type.Double);
                break;
            case BACNET_APPLICATION_TAG_OCTET_STRING:
                for (i = 0; i < value->type.Octet_String.length; i++) {
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "%02X/", value->type.Octet_String.value[i]);
                }
                break;
            case BACNET_APPLICATION_TAG_CHARACTER_STRING:
                snprintf(buffer, sizeof(buffer), "%s", value->type.Character_String.value);
                break;
            case BACNET_APPLICATION_TAG_BIT_STRING:
                for (i = 0; i < value->type.Bit_String.bits_used; i++) {
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "%d", (value->type.Bit_String.value[i / 8] >> (7 - (i % 8))) & 1);
                }
                break;
            case BACNET_APPLICATION_TAG_ENUMERATED:
                snprintf(buffer, sizeof(buffer), "%u", value->type.Enumerated);
                break;
            case BACNET_APPLICATION_TAG_DATE:
                snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u", value->type.Date.year, value->type.Date.month, value->type.Date.day);
                break;
            case BACNET_APPLICATION_TAG_TIME:
                snprintf(buffer, sizeof(buffer), "%02u:%02u:%02u.%02u", value->type.Time.hour, value->type.Time.min, value->type.Time.sec, value->type.Time.hundredths);
                break;
            case BACNET_APPLICATION_TAG_OBJECT_ID:
                snprintf(buffer, sizeof(buffer), "Type: %u, Instance: %u", value->type.Object_Id.type, value->type.Object_Id.instance);
                break;
            case BACNET_APPLICATION_TAG_EMPTYLIST:
                break;
            case BACNET_APPLICATION_TAG_DATETIME:
                snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u %02u:%02u:%02u.%02u", 
                    value->type.Date_Time.date.year, value->type.Date_Time.date.month, value->type.Date_Time.date.day,
                    value->type.Date_Time.time.hour, value->type.Date_Time.time.min, value->type.Date_Time.time.sec, value->type.Date_Time.time.hundredths);
                break;
            case BACNET_APPLICATION_TAG_DATERANGE:
                snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u to %04u-%02u-%02u", 
                    value->type.Date_Range.startdate.year, value->type.Date_Range.startdate.month, value->type.Date_Range.startdate.day,
                    value->type.Date_Range.enddate.year, value->type.Date_Range.enddate.month, value->type.Date_Range.enddate.day);
                break;
            case BACNET_APPLICATION_TAG_LIGHTING_COMMAND:
                snprintf(buffer, sizeof(buffer), "Operation: %u, Target Level: %f, Ramp Rate: %f, Step Increment: %f, Fade Time: %u, Priority: %u", 
                    value->type.Lighting_Command.operation, 
                    value->type.Lighting_Command.target_level, 
                    value->type.Lighting_Command.ramp_rate, 
                    value->type.Lighting_Command.step_increment, 
                    value->type.Lighting_Command.fade_time, 
                    value->type.Lighting_Command.priority);
                break;
            case BACNET_APPLICATION_TAG_XY_COLOR:
                snprintf(buffer, sizeof(buffer), "X: %f, Y: %f", value->type.XY_Color.x_coordinate, value->type.XY_Color.y_coordinate);
                break;
            case BACNET_APPLICATION_TAG_COLOR_COMMAND:
                snprintf(buffer, sizeof(buffer), "Operation: %u, Color: X: %f, Y: %f", 
                    value->type.Color_Command.operation, 
                    value->type.Color_Command.target.color.x_coordinate, 
                    value->type.Color_Command.target.color.y_coordinate);
                break;
            case BACNET_APPLICATION_TAG_WEEKLY_SCHEDULE:
                break;
            case BACNET_APPLICATION_TAG_CALENDAR_ENTRY:
                //value->type.Calendar_Entry);
                break;
            case BACNET_APPLICATION_TAG_SPECIAL_EVENT:
                //value->type.Special_Event);
                break;
            case BACNET_APPLICATION_TAG_HOST_N_PORT:
                if (value->type.Host_Address.host_ip_address) {
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "IP: ");
                    for (i = 0; i < value->type.Host_Address.host.ip_address.length; i++) {
                        snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "%02X ", value->type.Host_Address.host.ip_address.value[i]);
                    }
                } else if (value->type.Host_Address.host_name) {
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Name: %s", value->type.Host_Address.host.name.value);
                }
                snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Port: %u", value->type.Host_Address.port);
                break;
            case BACNET_APPLICATION_TAG_DEVICE_OBJECT_PROPERTY_REFERENCE:
                snprintf(buffer, sizeof(buffer), "Object ID: %u, Property ID: %u, Array Index: %u, Device ID: %u", 
                    value->type.Device_Object_Property_Reference.objectIdentifier.instance,
                    value->type.Device_Object_Property_Reference.propertyIdentifier,
                    value->type.Device_Object_Property_Reference.arrayIndex,
                    value->type.Device_Object_Property_Reference.deviceIdentifier.instance);
                break;
            case BACNET_APPLICATION_TAG_DEVICE_OBJECT_REFERENCE:
                snprintf(buffer, sizeof(buffer), "Device ID: %u, Object ID: %u",
                    value->type.Device_Object_Reference.deviceIdentifier.instance,
                    value->type.Device_Object_Reference.objectIdentifier.instance);
                break;
            case BACNET_APPLICATION_TAG_OBJECT_PROPERTY_REFERENCE:
                snprintf(buffer, sizeof(buffer), "Object ID: %u, Property ID: %u, Array Index: %u",
                    value->type.Object_Property_Reference.object_identifier.instance,
                    value->type.Object_Property_Reference.property_identifier,
                    value->type.Object_Property_Reference.property_array_index);
                break;
            case BACNET_APPLICATION_TAG_DESTINATION:
                snprintf(buffer, sizeof(buffer), "Process Identifier: %u, Confirmed Notify: %s", 
                    value->type.Destination.ProcessIdentifier, value->type.Destination.ConfirmedNotify ? "true" : "false");
                break;
            default:
                break;
        }
    }
}

void sync_read(BACNET_READ_PROPERTY_DATA *rpdata) {
    if (!check_object_type(rpdata->object_type)) {
        return;
    }
    char *msg = generate_str("READ", rpdata->object_type, rpdata->object_property, NULL);
    if (msg == "") {
        return;
    }
    char *output = write_to_socket(msg);
    BACNET_CHARACTER_STRING char_string;
    if (strlen(output) != 0){
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
        encode_application_character_string(&wp_data.application_data, &char_string);
        uint32_t i;
        size_t length = characterstring_length(&char_string)+3;
        wp_data.application_data_len = length;

        Device_Write_Property(&wp_data);
    }
}

void sync_write(BACNET_WRITE_PROPERTY_DATA *wpdata) {
    if (!check_object_type(wpdata->object_type)) {
        return;
    }
    BACNET_APPLICATION_DATA_VALUE value;
    bacapp_decode_application_data(wpdata->application_data, wpdata->application_data_len, &value);
    char buffer[1024];
    parse_property_string(&value, buffer);
    char *msg = generate_str("WRITE", wpdata->object_type, wpdata->object_property, buffer);
    write_to_socket(msg);
}

char *write_to_socket(const char* message) {
    int sockfd;
    struct sockaddr_un serv_addr;

    // create socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        return;
    }

    // init socket server struct
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SOCKET_PATH);

    // connect to server
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to socket");
        return;
    }

    // write to socket
    write(sockfd, message, strlen(message));

    // read from socket
    char *buffer = malloc(512); 
    int n = read(sockfd, buffer, 512);
    if (n < 0) {
        perror("Error reading from socket");
        free(buffer);
        return;
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