# via https://stackoverflow.com/questions/71479189/shared-memory-ipc-solution-for-both-linux-and-windows
from multiprocessing import shared_memory

def main():
  # create=false, use existing
  # 10 is the # bytes, likely to change
  shm_a = shared_memory.SharedMemory(name="ReimaginingBreath", size=10)
  while 1:
    # Reads in byte array, convert that to integer using little endian.
    print(int.from_bytes(bytes(shm_a.buf[:10]), 'little'))

  shm_a.close()

if __name__ == "__main__":
         main()
