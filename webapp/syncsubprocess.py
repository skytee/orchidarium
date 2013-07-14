import threading, subprocess
from time import sleep
import syslog

# Provides serialized command execution.
# Once call() is called all subsequent call attempts block until first call finishes.
# This is true for calls from same or different threads (not reentrant).
# Thorsten Haas, <dev@thorstenhaas.de>
class syncsubprocess:

	_lock = threading.Lock()

	def __init__(self):
		syslog.openlog()

	def __del__(self):
		syslog.closelog()


	# issue command, read stdout/stderr, wait for termination and fetch return code
	def call(self, cmd):
		print "extcall:", cmd
		self._lock.acquire()
		print "extcall:", cmd, "executing"
		try:
			for retries in range(1, 3):
				p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
				pOutput = p.stdout.readlines()
				retval = p.wait()
				if retval == 0:
					break # retval good, thus break out of loop
				else:
					print "extcall:", cmd, "RETRY", retries
					sleep(0.30) # retry after short nap
				if retries == 2:
					logstr = "failed to execute '" + cmd  + "'\n"
					syslog.syslog(logstr)
					break # too many retries. give up.
		finally:
			self._lock.release()
		return pOutput, retval

shell = syncsubprocess()

