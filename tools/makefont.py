import sys

class Make:
	def __init__(self, fin, fout):
		self.fin = open(fin, "r")
		self.ofn = fout
		self.fout = open(fout, "wb+");

	def make(self):
		self.fout.write(b"char MonkeyOSMainFont[] = {\n")
		while 1:
			l = self.fin.readline()
			if not l:
				break
			if l[:4] == "char":
				self.fout.write(b"\t")
				for i in range(16):
					now = 0
					for idx, j in enumerate(self.fin.readline()):
						if j == "*":
							now += 1 << (7-idx)
					self.fout.write((hex(now) + ",\t").encode())
				self.fout.write(b"\n")
		self.fout.write(b"\t0x00\n};\n")

Make(sys.argv[1], sys.argv[2]).make()
