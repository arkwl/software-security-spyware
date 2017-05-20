import subprocess 
import socket
import sys

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(('localhost', 9999))

while True:
	pid = 20000

	while pid < 40000:
		proc = subprocess.Popen(['sudo', 'strace', '-c', '-p', str(pid)], stdout=subprocess.PIPE)
		tmp = proc.stdout.read()
		print client.send(tmp), 'bytes sent.'
		pid = pid + 1

#sudo strace -c -p 12345
