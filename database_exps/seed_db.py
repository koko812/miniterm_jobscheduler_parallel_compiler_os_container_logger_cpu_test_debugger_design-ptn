import sqlite3
import random
import json

conn = sqlite3.connect("game.db")
cur = conn.cursor()

cur.execute("""
CREATE TABLE IF NOT EXISTS players (
  id INTEGER PRIMARY KEY,
  name TEXT,
  level INTEGER,
  hp INTEGER,
  gold INTEGER
)
""")

cur.execute("""
CREATE TABLE IF NOT EXISTS items (
  id INTEGER PRIMARY KEY,
  name TEXT,
  rarity TEXT,
  power INTEGER
)
""")

cur.execute("""
CREATE TABLE IF NOT EXISTS inventories (
  player_id INTEGER,
  item_id INTEGER,
  qty INTEGER,
  PRIMARY KEY (player_id, item_id)
)
""")

# players
# players
for pid in range(1, 10001):
    cur.execute(
        "INSERT OR REPLACE INTO players VALUES (?, ?, ?, ?, ?)",
        (pid, f"player{pid}", random.randint(1, 30), random.randint(50, 150), random.randint(0, 5000))
    )


# items
rarities = ["common", "rare", "epic"]
for iid in range(1, 11):
    cur.execute(
        "INSERT OR REPLACE INTO items VALUES (?, ?, ?, ?)",
        (iid, f"item{iid}", random.choice(rarities), random.randint(1, 100))
    )

# inventories
for pid in range(1, 10001):
    for _ in range(5):
        item_id = random.randint(1, 10)
        qty = random.randint(1, 3)
        cur.execute(
            "INSERT OR REPLACE INTO inventories VALUES (?, ?, ?)",
            (pid, item_id, qty)
        )

conn.commit()
conn.close()
print("seeded game.db")
