from os import system as s
from sys import argv

_cmd = ""
for i in argv[1:]:
	_cmd += i + " "

print(_cmd)
s(_cmd)
