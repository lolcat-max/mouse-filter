#include <iostream>
#include <windows.h>
#include "interception.h"

int main() {
    InterceptionContext context;
    InterceptionDevice device;
    InterceptionMouseStroke mstroke; // Use this for mouse events

    // Hide the console window to run silently in the background
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Create a context with the Interception driver
    context = interception_create_context();
    if (context == NULL) {
        std::cerr << "Failed to create Interception context." << std::endl;
        return -1;
    }

    // Tell the driver we want to intercept all mouse events
    interception_set_filter(context, interception_is_mouse, INTERCEPTION_FILTER_MOUSE_ALL);

    std::cout << "Filter started. Canceling absolute mouse movements." << std::endl;

    // Main loop: wait for an event, check it, and decide what to do
    while (interception_receive(context, device = interception_wait(context), (InterceptionStroke*)&mstroke, 1) > 0) {

        // Check if the received stroke is from a mouse
        if (interception_is_mouse(device)) {

            // This is the core logic:
            // Check the flags for the INTERCEPTION_MOUSE_MOVE_ABSOLUTE bit.
            if (mstroke.flags & INTERCEPTION_MOUSE_MOVE_ABSOLUTE) {
                // If it IS an absolute movement, we do nothing.
                // By not sending it back, we effectively "cancel" it.
            } else {
                // If it's NOT absolute (i.e., it's a regular relative movement),
                // we send it right back to the OS to be processed normally.
                interception_send(context, device, (InterceptionStroke*)&mstroke, 1);
            }
        }
    }

    // Clean up when the program exits
    interception_destroy_context(context);
    return 0;
}
