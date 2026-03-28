import redis

r = redis.Redis(host="localhost", port=6379, decode_responses=True)

r.hset("player:1", mapping={"x": 10, "y": 20, "hp": 100})

print(r.hget("player:1", "x"))
print(r.hgetall("player:1"))

r.lpush("match:1:log", "player1 joined")
r.lpush("match:1:log", "player2 joined")

print(r.lrange("match:1:log", 0, -1))

r.zadd("leaderboard", {"alice": 100, "bob": 80})
r.zincrby("leaderboard", 50, "bob")

print(r.zrevrange("leaderboard", 0, -1, withscores=True))