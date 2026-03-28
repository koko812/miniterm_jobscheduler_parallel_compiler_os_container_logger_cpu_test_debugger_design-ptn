# db_read.py
import sqlite3

conn = sqlite3.connect("game.db")
cur = conn.cursor()

# players を全部見る
cur.execute("SELECT id, name, level, hp, gold FROM players")
for row in cur.fetchall():
    print(row)

# 特定プレイヤー
pid = 1
cur.execute("SELECT * FROM players WHERE id = ?", (pid,))
print("player1:", cur.fetchone())

conn.close()