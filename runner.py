from subprocess import Popen, PIPE

class FilesNotEqual(Exception):
	pass
try:
    i = int(input("Iterations(must be integer number): "))
except ValueError:
    print("ValueError: Please enter integer next time")
    exit()

program = "./counter"

result_lst = []

for i in range(i):
    result = Popen(program, stdout=PIPE)
    out = result.stdout.read()
    x = str(out, "utf-8")
    result_lst.append(int(x))

print("Results:", result_lst)
print("Minimum result: ", min(result_lst))
