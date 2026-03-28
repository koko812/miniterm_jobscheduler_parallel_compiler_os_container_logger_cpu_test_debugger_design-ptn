import redis
import time

r = redis.Redis(host="localhost", port=6379, decode_responses=True)
r.set("foo", "bar")
print("GET foo =", r.get("foo"))

r.incr("count")
print("count =", r.get("count"))

r.set("temp", "hello", ex=3)
print("temp =", r.get("temp"))

print("sleep 1")
time.sleep(1)
print("sleep 2")
time.sleep(1)
print("sleep 3")
time.sleep(1)

print("GET foo =", r.get("foo"))
print("count =", r.get("count"))
print("temp =", r.get("temp"))

