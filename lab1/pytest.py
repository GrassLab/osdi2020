# -*- coding: utf-8 -*-
from __future__ import absolute_import
from __future__ import print_function
from __future__ import unicode_literals
import pexpect
import sys

image = "kernel8.img"

cmd = "qemu-system-aarch64 "
cmd += "-M raspi3 -kernel %s " % image
cmd += "-serial null -serial stdio"

child = pexpect.spawnu(cmd)
child.logfile_read = sys.stdout

with open("pytest_cmd", "r") as f:
    line = f.readline().strip('\n')
    while line:
        print(line)
        child.expect("# ")
        child.sendline(line)
        line = f.readline().strip('\n')

child.expect("# ")
# interact mode
child.logfile_read = None
child.interact()

