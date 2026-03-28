import sys
import time
import threading
import urllib.request

URL = sys.argv[1] if len(sys.argv) > 1 else "http://localhost:3000/hello"
TOTAL = int(sys.argv[2]) if len(sys.argv) > 2 else 2000
CONCURRENCY = int(sys.argv[3]) if len(sys.argv) > 3 else 50
MAX_RETRIES = int(sys.argv[4]) if len(sys.argv) > 4 else 0
BACKOFF_MS = int(sys.argv[5]) if len(sys.argv) > 5 else 50

lock = threading.Lock()
count = 0
errors = 0
retries = 0
durations = []


def request_once():
    with urllib.request.urlopen(URL, timeout=10) as r:
        r.read()


def worker(n):
    global count, errors, retries
    for _ in range(n):
        start = time.time()
        attempt = 0
        while True:
            try:
                request_once()
                break
            except Exception:
                if attempt < MAX_RETRIES:
                    attempt += 1
                    with lock:
                        retries += 1
                    time.sleep(BACKOFF_MS / 1000.0 * attempt)
                    continue
                with lock:
                    errors += 1
                break
            finally:
                dur = time.time() - start
                with lock:
                    durations.append(dur)
                    count += 1


per_thread = TOTAL // CONCURRENCY
threads = []


t0 = time.time()
for _ in range(CONCURRENCY):
    t = threading.Thread(target=worker, args=(per_thread,))
    t.start()
    threads.append(t)

for t in threads:
    t.join()

elapsed = time.time() - t0
durations.sort()


def pct(p):
    if not durations:
        return 0.0
    i = int(len(durations) * p)
    i = min(max(i, 0), len(durations) - 1)
    return durations[i]


print(f"total: {count}, errors: {errors}, retries: {retries}, elapsed: {elapsed:.3f}s")
print(f"rps: {count/elapsed:.1f}")
print(f"p50: {pct(0.50):.4f}s, p95: {pct(0.95):.4f}s, p99: {pct(0.99):.4f}s")
