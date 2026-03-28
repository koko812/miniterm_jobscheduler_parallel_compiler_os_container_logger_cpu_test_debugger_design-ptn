from http.server import BaseHTTPRequestHandler, HTTPServer
import sqlite3
import urllib.parse
import time
import redis

DB_PATH = "game.db"
r = redis.Redis(host="localhost", port=6379, decode_responses=True)

def fetch_player(pid: int):
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()
    cur.execute("SELECT id, name, level, hp, gold FROM players WHERE id = ?", (pid,))
    row = cur.fetchone()
    conn.close()
    return row

def fetch_top100():
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()
    cur.execute("SELECT id, name, level, hp, gold FROM players ORDER BY gold DESC LIMIT 100")
    rows = cur.fetchall()
    conn.close()
    return rows

class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path.startswith("/leaderboard"):
            cache_key = "leaderboard:top100"
            cached = r.get(cache_key)
            if cached:
                body = cached + "\n"
                self.send_response(200)
                self.end_headers()
                self.wfile.write(body.encode())
                return

            time.sleep(1.0)  # DB遅延の再現
            rows = fetch_top100()
            body = "\n".join([f"{r[0]} {r[1]} gold={r[4]}" for r in rows])
            r.set(cache_key, body, ex=10)  # 10秒キャッシュ

            self.send_response(200)
            self.end_headers()
            self.wfile.write((body + "\n").encode())
            return

        # 通常: /?id=1
        qs = urllib.parse.urlparse(self.path).query
        params = urllib.parse.parse_qs(qs)
        pid = int(params.get("id", ["1"])[0])

        time.sleep(0.5)  # DB遅延の再現
        row = fetch_player(pid)
        if row is None:
            self.send_response(404)
            self.end_headers()
            self.wfile.write(b"not found\n")
            return

        body = f"id={row[0]}, name={row[1]}, level={row[2]}, hp={row[3]}, gold={row[4]}\n"
        self.send_response(200)
        self.end_headers()
        self.wfile.write(body.encode())

def main():
    server = HTTPServer(("0.0.0.0", 8002), Handler)
    print("listening on http://localhost:8002")
    server.serve_forever()

if __name__ == "__main__":
    main()
