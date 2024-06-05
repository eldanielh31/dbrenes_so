#include <stdio.h>
#include "arduino_lib.h"

int main() {
    const char *message = "Hola mundo\n";
    int result = send_to_arduino(message);

    if (result < 0) {
        printf("Failed to send message to Arduino\n");
    } else {
        printf("Message sent successfully\n");
    }

    return 0;
}
