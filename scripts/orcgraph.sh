#!/bin/bash

END=$(rrdtool fetch /var/tmp/orchidarium01.rrd AVERAGE | grep -v 'nan nan nan nan nan' | tail -1 | cut -f 1 -d ':')
rrdtool graph /srv/www/static/orchidarium_graph.png \
--end $END \
DEF:temperature=/var/tmp/orchidarium01.rrd:temperature:AVERAGE \
DEF:humidity=/var/tmp/orchidarium01.rrd:humidity:AVERAGE \
DEF:light=/var/tmp/orchidarium01.rrd:light:AVERAGE \
DEF:fan=/var/tmp/orchidarium01.rrd:fan:AVERAGE \
DEF:mist=/var/tmp/orchidarium01.rrd:mist:AVERAGE \
DEF:illuminance=/var/tmp/orchidarium01.rrd:illuminance:AVERAGE \
CDEF:humidity_scaled=humidity,0.5,* \
CDEF:light_on=light,.5,GT,6,5,IF \
CDEF:fan_on=fan,.5,GT,8,7,IF \
CDEF:mist_on=mist,.5,GT,10,9,IF \
--title "Orchidarium Stats" \
--width 600 \
--height 150 \
--right-axis 2.0:0 --right-axis-label "humidity in %rel" \
--legend-position=south \
--border 0 \
--vertical-label "temperature in °C" \
LINE:illuminance#ffa81f:"Illuminance" \
LINE:humidity_scaled#0000EE:"Humidity" \
LINE:fan_on#666666:"Fan (on/off)" \
LINE:light_on#a242fb:"Light (on/off)" \
LINE:mist_on#4ea736:"Mist (on/off)" \
LINE:temperature#FF0000:"Temperature" \
>/dev/null

