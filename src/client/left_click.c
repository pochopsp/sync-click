#include <windows.h>
#include "usleep.h"


void doDelayedLeftClick(unsigned int clickDelay){
    usleep(clickDelay);

    INPUT events[2];

    MOUSEINPUT leftClickPress = {.dwFlags = MOUSEEVENTF_LEFTDOWN};
    INPUT click = {.type = INPUT_MOUSE, .mi = leftClickPress};
    
    MOUSEINPUT leftClickRelease = {.dwFlags = MOUSEEVENTF_LEFTUP};
    INPUT release = {.type = INPUT_MOUSE, .mi = leftClickRelease};

    events[0] = click;
    events[1] = release;
    
    int mouseEventsCount = sizeof(events)/sizeof(INPUT);
    SendInput(mouseEventsCount, events, sizeof(INPUT));
}