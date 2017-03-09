#!/usr/bin/python
import subprocess
from signal import SIGTERM
import time
import sys
import os

def main(arr, secs_to_wait):
	print(arr[0])
	pipe = subprocess.Popen([sys.executable, arr[0], arr[1]])
	# time to sleep in seconds.
	time.sleep(secs_to_wait)
	pipe.poll()
	if pipe.returncode == None:
		print("[%s] stop\n" % pipe.pid)


		# give the monitored program a chance to flush files,
		# wait for 10 seconds.
		os.kill(pipe.pid, SIGTERM)
		time.sleep(10)
		pipe.poll()
		if pipe.returncode == None:
			print("[%s] kill\n" % pipe.pid)
			os.kill(pipe.pid, sig)

main(sys.argv[1:], 3*60)

# usage:
#   ./watch.py <program> <arguments>
#   e.g
#   ./watch.py nearest_neighbor.py tsp_example_1.txt

