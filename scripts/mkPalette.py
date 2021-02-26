dat = "0123456789abcdef"
x = 0
def make_a_num():
	for i in dat:
		for j in dat:
			yield "0x%s%s" % (i, j)

def make():
	for i in make_a_num():
		for j in make_a_num():
			for p in make_a_num():
				yield "%s, %s, %s\n" % (i, j, p)

with open("a.txt", "w+") as f:
	for k in make():
		f.write(k)