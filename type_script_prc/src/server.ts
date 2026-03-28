import * as http from "node:http";

const server = http.createServer((req, res) => {
    if (req.url == '/hello'){
        res.writeHead(200, {"Content-Type": "application/json"});
        res.end(JSON.stringify({message:"hello_world"})); 
        return;
    }

    res.writeHead(404, {"Content-Type": "application/json"});
    res.end(JSON.stringify({error:"not found"})); 
})

server.listen(3000, ()=> {
    console.log("listening on http://localhost:3000")
})