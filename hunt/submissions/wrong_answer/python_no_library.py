# The library exists only for C++, so a submission in any other language never
# answers the validator's handshake and cannot be scored. It gets Wrong Answer
# with an explanation rather than a mysterious hang.
import sys

print("1 1 1000000 3 33 3 1 1000000 3 33 3")
sys.stdout.flush()
