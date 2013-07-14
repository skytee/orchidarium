#!/usr/bin/env python
# sispmctl wrapper script. Makes the crontab a lot more readable.
import getopt, sys, subprocess
from time import sleep

def main():
	try:
		opts, args = getopt.getopt(sys.argv[1:], "l:m:f:", ["light=", "mist=", "fan="])
	except getopt.GetoptError as err:
		# print help information and exit:
		print str(err) # will print something like "option -a not recognized"
		usage()
		sys.exit(2)
	output = None
	verbose = False
	cmd = ''
	for o, a in opts:
		if o in ("-l", "--light"):
			if a == "on":
				cmd += ' -o 1' 
			elif a == "off":
				cmd += ' -f 1' 
			else:
				sys.stderr.write("unhandled argument to " + o + ": " + a +"\n")
				sys.exit(2)
		elif o in ("-f", "--fan"):
			if a == "on":
				cmd += ' -o 2' 
			elif a == "off":
				cmd += ' -f 2' 
			else:
				sys.stderr.write("unhandled argument to " + o + ": " + a +"\n")
				sys.exit(2)
		elif o in ("-m", "--mist"):
			if a == "on":
				cmd += ' -o 3' 
			elif a == "off":
				cmd += ' -f 3' 
			else:
				sys.stderr.write("unhandled argument to " + o + ": " + a +"\n")
				sys.exit(2)
		else:
			sys.exit(2)
	if cmd == '':
		sys.stderr.write("nothing to do\n")
		sys.exit(2)
	else:
		cmd = 'sispmctl -nq' + cmd

	cmdOutput = {}
	cmdOutput, retval = extcall(cmd)
	if retval != 0:
		for line in cmdOutput:
			sys.stderr.write(line)

	sys.exit(retval)

def extcall(cmd):
	# issue command, read stdout/stderr, wait for termination and fetch return code
	for retries in range(1, 3):
		p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		pOutput = p.stdout.readlines()
		retval = p.wait()
		if retval == 0:
			break # retval good, thus break out of loop
		else:
			sleep(0.30) # retry after short nap
		if retries > 3:
			break # too many retries. give up.
	return pOutput, retval

if __name__ == "__main__":
	main()
