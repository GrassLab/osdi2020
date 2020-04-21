import sys
import threading
import time
import queue

def add_input(input_queue):
    while True:
        input_queue.put(sys.stdin.readline())

def foobar():
    input_queue = queue.Queue()

    input_thread = threading.Thread(target=add_input, args=(input_queue,))
    input_thread.daemon = True
    input_thread.start()

    last_update = time.time()
    while True:

        if time.time()-last_update>0.5:
            sys.stdout.write(".")
            sys.stdout.flush() 
            last_update = time.time()

        if not input_queue.empty():
            print("\ninput:", input_queue.get())

foobar()