# An example of how to use the timer. 
#
# The timer is run in the bottom center part of the screen
#
# First run the timer for 26 minutes, then play a fanfare with mplayer, and finally run the counter for 5 minutes in yellow as the five-minute break.

./osd_pomodoro -S 1560 -m && mplayer fanfare.mp3 && ./osd_pomodoro -S 300 -c yellow -m
