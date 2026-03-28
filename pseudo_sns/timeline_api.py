from http.server import BaseHTTPRequestHandler, HTTPServer
import sqlite3
import urllib.parse
from datetime import datetime, timedelta, timezone

DB_PATH = "sns.db"

def fetch_timeline(user_id: int, limit: int = 20):
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()
    cur.execute("""
        SELECT p.id, p.user_id, p.body, p.created_at
        FROM posts p
        JOIN follows f ON p.id = f.followee_id
        WHERE f.follower_id = ?
        ORDER BY p.created_at DESC
        LIMIT ?;
    """, (user_id, limit))
    rows = cur.fetchall()
    conn.close()
    return rows

def add_follow(follower_id: int, followee_id: int):
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()
    cur.execute(
        "INSERT OR IGNORE INTO follows (follower_id, followee_id, created_at) VALUES (?,?,?)",
        follower_id, followee_id, datetime.now(timezone.utc).isoformat()
    )
    conn.commit()
    conn.close()

class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        if not self.path.startswith("/timeline"):
            self.send_response(404)
            self.end_headers()
            self.wfile.write(b"not found\n")
            return

        qs = urllib.parse.urlparse(self.path).query
        params = urllib.parse.parse_qs(qs)
        uid = int(params.get("user_id", ["1"])[0])
        limit = int(params.get("limit", ["20"])[0])

        rows = fetch_timeline(uid, limit)
        body = "\n".join([f"{r[0]} {r[1]} {r[2]} {r[3]}" for r in rows]) + "\n"
        self.send_response(200)
        self.end_headers()
        self.wfile.write(body.encode())

def main():
    server =  HTTPServer(("0.0.0.0", 8004), Handler)
    print("listening on http://localhost:8004")
    server.serve_forever()

if __name__ == "__main__":
    main()