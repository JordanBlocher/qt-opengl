#!/usr/bin/python

import os
import re
import sys

# list of extensions to replace
replace_extensions = []
# example: uncomment next line to only replace *.c, *.h, and/or *.txt
# replace_extensions = [".c", ".h", ".txt"]

def try_to_replace(fname):
    if replace_extensions:
        return fname.lower().endswith(replace_extensions)
    return True


def file_replace(fname, pat, s_after):
    # first, see if the pattern is even in the file.
    with open(fname) as f:
        if not any(re.search(pat, line) for line in f):
            return # pattern does not occur in file so we are done.

    # pattern is in the file, so perform replace operation.
    with open(fname) as f:
        out_fname = fname + ".tmp"
        out = open(out_fname, "w")
        for line in f:
            out.write(re.sub(pat, s_after, line))
        out.close()
        os.rename(out_fname, fname)


def mass_replace(dir_name, s_before, s_after):
    pat = re.compile(s_before)
    for dirpath, dirnames, filenames in os.walk(dir_name):
        for fname in filenames:
            if try_to_replace(fname):
                fullname = os.path.join(dirpath, fname)
                file_replace(fullname, pat, s_after)

if len(sys.argv) != 4:
    u = "Usage: mass_replace <dir_name> <string_before> <string_after>\n"
    sys.stderr.write(u)
    sys.exit(1)


mass_replace(sys.argv[1], sys.argv[2], sys.argv[3]);
