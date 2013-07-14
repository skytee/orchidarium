#!/bin/bash

DATE=$(date +%Y%m%d)

lftp -q -c "set ftp:list-options -a;
open ftp://anonymous:password@pho;
cd /ftp-incoming;
lcd /var/tmp;
put orchidarium01.rrd -o $DATE-orchidarium01.rrd
lcd /srv/www/static
put orchidarium_graph.png -o $DATE-orchidarium_graph.png" >/dev/null
