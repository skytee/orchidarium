import threading

# Provide a self-synchronising container object, i.e.
# once one of its methods is called all subsequent call attempts block until first call finishes.
# This is true for calls from same or different threads (not reentrant).
# Thorsten Haas, <dev@thorstenhaas.de>
class lockingcache:
	_cache = {}
	_cacheLock = threading.Lock()

	def get(self):
		cache = {}
		self._cacheLock.acquire()
		try:
			cache = self._cache
		finally:
			self._cacheLock.release()
		return cache
	
	def set(self, cache):
		self._cacheLock.acquire()
		try:
			self._cache = cache
		finally:
			self._cacheLock.release()

power = lockingcache()
sensors = lockingcache()
