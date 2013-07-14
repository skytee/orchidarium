orchidarium - webapp
====================

Runs a background thread that periodically reads sensor data and power outlet states. This data is both contained in a self-synchronising object and sent to an [RRD](http://oss.oetiker.ch/rrdtool/).

Provides a UI to:
* let the user manually switch the power outlets
* show current sensor data and power outlet states (light, fan, mist)
* show a graph based on the RRD
* display the orchidarium's crontab

