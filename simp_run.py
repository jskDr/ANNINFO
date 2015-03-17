# Modified - from pyfann import libfann (jskDr, 2015-3-17)
# Original - http://jansipke.nl/using-fann-with-python/

import libfann

ann = libfann.neural_net()
ann.create_from_file("xor.net")

print ann.run([1, -1])
