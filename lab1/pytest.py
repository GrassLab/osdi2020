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

child.expect("# ") 
child.sendline("help")

child.expect("# ")
child.sendline("hello")

child.expect("# ")
child.sendline("timestamp")

child.expect("# ")
child.sendline("reboot")

child.expect("# ")
# interact mode
child.logfile_read = None
child.interact()

