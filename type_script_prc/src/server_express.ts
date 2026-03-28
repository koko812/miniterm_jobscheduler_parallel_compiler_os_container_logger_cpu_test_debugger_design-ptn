import express from "express";
import * as http from "node:http";
import crypto from "node:crypto";
import { promisify } from "node:util";

const ENABLE_LOG = process.env.LOG_REQUESTS === "1";

const app = express();

if (ENABLE_LOG) {
    app.use((req, res, next) => {
        const start = Date.now();
        res.on("finish", () => {
            const ms = Date.now() - start;
            console.log(`${req.method} ${req.url} -> ${res.statusCode} (${ms}ms)`)
        });
        next();
    }) 
}

app.get("/hash", (req,res) => {
    const input = String(req.query.input ?? "");
    const hash = crypto.createHash("sha256").update(input).digest("hex");
    res.json({input, hash, algo: "sha256"})
})

app.get("/heavy_hash", (req, res) => {
    const input = String(req.query.input ?? "");
    const salt = "demo-salt";
    const iterations = 100000;
    const keylen = 32;
    const digest = "sha256"

    const hash = crypto
        .pbkdf2Sync(input, salt, iterations, keylen, digest)
        .toString("hex");

    res.json({input, hash, algo: "pbkdf2", iterations, digest})
})

const pbkdf2Async = promisify(crypto.pbkdf2);

app.get("/a_heavy_hash", async (req, res) => {
    const input = String(req.query.input ?? "");
    const salt = "demo-salt";
    const iterations = 10000;
    const keylen = 128;
    const digest = "sha256";

    const hashBuffer = await pbkdf2Async(input, salt, iterations, keylen, digest);
    const hash = hashBuffer.toString("hex");

    res.json({input, hash, algo: "pbkdf2", iterations, digest});
})

// グローバルに保持（メモリが増え続けるので注意）
const memStore: Buffer[] = [];

app.get("/mem_alloc", (req, res) => {
    const mb = Number(req.query.mb ?? 10); // 追加で確保するMB
    const count = Math.max(1, Math.floor(mb));
    for (let i = 0; i < count; i++) {
        memStore.push(Buffer.alloc(1024 * 1024)); // 1MB
    }
    res.json({ addedMB: count, totalMB: memStore.length });
});

app.get("/mem_clear", (req, res) => {
    memStore.length = 0;
    res.json({ cleared: true });
});

app.get("/mem_hold", (req, res) => {
    const mb = Math.max(1, Number(req.query.mb ?? 1000));
    const sec = Math.max(1, Number(req.query.sec ?? 10));

    const chunks: Buffer[] = [];
    for (let i = 0; i < mb; i++) {
        chunks.push(Buffer.alloc(1024 * 1024)); // 1MB
    }

    setTimeout(() => {
        // 参照を切って解放対象にする
        chunks.length = 0;
    }, sec * 1000);

    res.json({ allocatedMB: mb, holdSec: sec });
});



app.get('/hello', (req, res)=>{
    res.json({message:"hello"})
})

const server = http.createServer((req, res) => {
    if (req.url == '/hello_http'){
        res.writeHead(200, {"Content-Type": "application/json"});
        res.end(JSON.stringify({message:"hello_world"})); 
        return;
    }

    res.writeHead(404, {"Content-Type": "application/json"});
    res.end(JSON.stringify({error:"not found"})); 
})

app.get('/hello_late', async (req, res)=>{
    await new Promise((r) => setTimeout(r, 1000));
    res.json({message:"hello"})
})

app.get('/hello_random_late', async (req, res)=>{
    const min = 200;
    const max = 1500;
    const ms = Math.floor(Math.random() * (max - min + 1)) + min;
    await new Promise((r) => setTimeout(r, ms));
    res.json({message:"hello"})
})

app.use((req, res) => {
    res.status(404).json({error: "not found"})
})

app.listen(3000, "0.0.0.0", 2048, ()=> {
    console.log("listening on 3000")
})
