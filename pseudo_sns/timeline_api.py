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
        LEFT JOIN follows f ON p.user_id = f.followee_id AND f.follower_id = ?
        WHERE f.follower_id IS NOT NULL OR p.user_id = ?
        ORDER BY p.created_at DESC
        LIMIT ?;
    """, (user_id, user_id, limit))
    rows = cur.fetchall()
    conn.close()
    return rows

def add_follow(follower_id: int, followee_id: int):
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()
    cur.execute(
        "INSERT OR IGNORE INTO follows (follower_id, followee_id, created_at) VALUES (?,?,?)",
        (follower_id, followee_id, datetime.now(timezone.utc).isoformat())
    )
    conn.commit()
    conn.close()

def add_post(user_id: int, body: str):
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()
    cur.execute(
        "INSERT INTO posts (user_id, body, created_at) VALUES (?,?,?)",
        (user_id, body, datetime.now(timezone.utc).isoformat())
    )
    conn.commit()
    conn.close()

def delete_post(user_id: int, post_id: int):
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()
    cur.execute(
        "DELETE FROM posts WHERE id=? AND user_id=?",
        (post_id, user_id)
    )
    conn.commit()
    conn.close()

def delete_follow(follower_id: int, followee_id: int):
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()
    cur.execute(
        "DELETE FROM follows WHERE follower_id=? AND followee_id=?",
        (follower_id, followee_id)
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


    def do_POST(self):
        if self.path.startswith("/follow"):
            qs = urllib.parse.urlparse(self.path).query
            params = urllib.parse.parse_qs(qs)
            follower_id = int(params.get("follower_id", ["1"])[0])
            followee_id = int(params.get("followee_id", ["2"])[0])

            add_follow(follower_id, followee_id)
            self.send_response(200)
            self.end_headers()
            self.wfile.write(b"ok\n")
            return

        if self.path.startswith("/unfollow"):
            qs = urllib.parse.urlparse(self.path).query
            params = urllib.parse.parse_qs(qs)
            follower_id = int(params.get("follower_id", ["1"])[0])
            followee_id = int(params.get("followee_id", ["2"])[0])

            delete_follow(follower_id, followee_id)

            self.send_response(200)
            self.end_headers()
            self.wfile.write(b"ok\n")
            return

        if self.path.startswith("/post"):
            qs = urllib.parse.urlparse(self.path).query
            params = urllib.parse.parse_qs(qs)
            user_id = int(params.get("user_id", ["1"])[0])
            body = params.get("body", ["hello"])[0]

            add_post(user_id, body)

            self.send_response(200)
            self.end_headers()
            self.wfile.write(b"ok\n")
            return

        if self.path.startswith("/delete_post"):
            qs = urllib.parse.urlparse(self.path).query
            params = urllib.parse.parse_qs(qs)
            user_id = int(params.get("user_id", ["1"])[0])
            post_id = int(params.get("post_id", ["1"])[0])

            delete_post(user_id, post_id)

            self.send_response(200)
            self.end_headers()
            self.wfile.write(b"ok\n")
            return

        self.send_response(404)
        self.end_headers()
        self.wfile.write(b"not found\n")
        return


def main():
    server =  HTTPServer(("0.0.0.0", 8004), Handler)
    print("listening on http://localhost:8004")
    server.serve_forever()

if __name__ == "__main__":
    main()