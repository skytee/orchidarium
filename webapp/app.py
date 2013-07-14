#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Web UI of @theorchidarium
# Thorsten Haas, <dev@thorstenhaas.de>
import web, json, re
from time import sleep
from thread import start_new_thread
import myvalue
import syncsubprocess
from math import * 

web.config.debug = True

urls = ('/', 'powercontrol')
render = web.template.render('templates/')

loopcount = 0

my_form = web.form.Form(
	web.form.Checkbox('Socket 1 - Light', value='none', id='socket1'),
	web.form.Checkbox('Socket 2 - Fan', value='none', id='socket2'),
	web.form.Checkbox('Socket 3 - Mist', value='none', id='socket3'),
	web.form.Checkbox('Socket 4', value='none', id='socket4'),
	web.form.Checkbox('Auto update', value='none', id='autoupdate', checked=False),
)

class powercontrol:
	def GET(self):
		form = my_form()
		matchObj = re.match('(.*):(.*)', web.ctx.ip,)
		if matchObj:
			infomsg = "Oh, an IPv6 visitor! " + web.ctx.ip;
		else:
			infomsg =''

		try:
			f = open('/etc/cron.d/orcron', 'r')
			crontab = f.read()
			f.close()
			crontab = re.sub("@\w+\.\w+", "@nospam", crontab)
		except (IOError):
			crontab = 'No crontab file.'
			pass

		return render.powercontrol(form, "Uninitialized.", infomsg, crontab)

	def POST(self): 
		# read input
		inp = web.input()
		# compile setting
		requests = {}
		for i in inp:
			#print "POST request got", i, inp[i]
			if i == 'socket1':
				requests['socket1'] = (inp[i] == 'true')
				setpower(requests)
			if i == 'socket2':
				requests['socket2'] = (inp[i] == 'true')
				setpower(requests)
			if i == 'socket3':
				requests['socket3'] = (inp[i] == 'true')
				setpower(requests)
			if i == 'socket4':
				requests['socket4'] = (inp[i] == 'true')
				setpower(requests)
		# return actual state
		web.header("Content-Type", "text/plain")
		return json.dumps(summary())


def summary():
	sensors = {}
	power = {}
	state = {}
	sensors = myvalue.sensors.get()
	power = myvalue.power.get()
	state = dict(sensors.items() + power.items())
	state['sysmsg'] = sensors['sysmsg'] + ' ' + power['sysmsg']
	return state

def readpower():
	powerDict = myvalue.power.get()
	cmdOutput = {}
	cmdOutput, retval = syncsubprocess.shell.call('sispmctl -nqg all')
	if retval != 0:
		powerDict = { 'sysmsg' : ' Socket powerDicts stale.'}
	else: 
		powerDict = { 'sysmsg' : ' Socket command ok.'}
		# command succeeded, assemble reply
		i = 1; # build index starting at 1st socket
		for line in cmdOutput:
			index = "socket" + `i`
			powerDict[index] = (line[:1] == '1') # sispmctl reply: 1st char is 0|1, representing off|on
			i += 1
	myvalue.power.set(powerDict)

def setpower(requests):
	# assemble command
	cmd = 'sispmctl -nq'
	for it in requests:
		socket = it[-1:]
		switchrequest = requests[it] # True or False
		switchcommand = ' -o ' if switchrequest else ' -f '
		cmd += switchcommand + socket 
	# execute
	syncsubprocess.shell.call(cmd)
	# update power status
	readpower()


def readsensors():
	sensorDict = myvalue.sensors.get()
	cmdOutput = {}
	cmdOutput, retval = syncsubprocess.shell.call('/usr/local/bin/orcsensors')
	if retval != 0:
		sensorDict['sysmsg'] = 'Sensor values stale.'
	else:
		sensorDict['sysmsg'] = 'Sensor command ok.'
		# command succeeded, assemble reply
		sensorDict['illuminance_ch0'] = cmdOutput[0].rstrip()
		sensorDict['illuminance_ch1'] = cmdOutput[1].rstrip()
		sensorDict['illuminance'] = cmdOutput[2].rstrip()
		sensorDict['humidity'] = cmdOutput[3].rstrip()
		sensorDict['temperature'] = cmdOutput[4].rstrip()
	myvalue.sensors.set(sensorDict)


# every n seconds
# trigger sensor and power readings
# and write readings to RRD
def sensorloop():
	global loopcount
	while True:
		readsensors()
		readpower()
		# write statistics every 30s
		loopcount += 1
		loopcount = loopcount % 15
		if loopcount == 0:
			sensors = myvalue.sensors.get()
			power = myvalue.power.get()
			cmd = 'rrdtool update -t temperature:humidity:illuminance:illuminance_ch0:illuminance_ch1:light:fan:mist /var/tmp/orchidarium01.rrd N:' + sensors.get('temperature', 'U') + ':' + sensors.get('humidity', 'U') + ':' + sensors.get('illuminance', 'U') + ':' + sensors.get('illuminance_ch0', 'U') + ':' + sensors.get('illuminance_ch1', 'U')
			for i in range(1, 4):
				index = "socket" + str(i)
				if power.get(index, False):
					cmd = cmd + ':1'
				else: 
					cmd = cmd + ':0'
			syncsubprocess.shell.call(cmd)
		sleep(2)
		


if __name__ == '__main__':
	app = web.application(urls, globals())
	start_new_thread(sensorloop,())
	app.run()

