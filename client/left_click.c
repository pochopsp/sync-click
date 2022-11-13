#include <windows.h>
#include <time.h>

void waitForSeconds (unsigned int secs) {
    if(secs == 0) return;
    // Get finishing time.
    unsigned int retTime = time(0) + secs;
    // Loop until it arrives.
    while (time(0) < retTime);
}

void doDelayedLeftClick(unsigned int clickDelay){
    waitForSeconds(clickDelay);

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