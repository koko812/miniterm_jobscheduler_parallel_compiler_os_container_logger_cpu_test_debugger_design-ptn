import sqlite3
from datetime import datetime, timedelta, timezone
import random

DB_PATH = "sns.db"

conn = sqlite3.connect(DB_PATH)
cur = conn.cursor()

for i in range(1,6):
    cur.execute(
        "INSERT OR REPLACE INTO users (id, username, created_at) VALUES (?,?,?)",
        (i, f"user{i}", datetime.now(timezone.utc).isoformat())
    )

cur.execute("INSERT OR REPLACE INTO follows VALUES (1,2,?)", (datetime.now(timezone.utc).isoformat(),))
cur.execute("INSERT OR REPLACE INTO follows VALUES (1,3,?)", (datetime.now(timezone.utc).isoformat(),))

base = datetime.now(timezone.utc)
pid = 1
for uid in range(1,6):
    for j in range(3):
        cur.execute(
            "INSERT OR REPLACE INTO posts (id, user_id, body, created_at) VALUES (?,?,?,?)",
            (pid, uid, f"post {pid} from user{uid}", (base-timedelta(minutes=pid)).isoformat())
        )

        pid += 1

conn.commit()
conn.close()
print("seeded sns.db")