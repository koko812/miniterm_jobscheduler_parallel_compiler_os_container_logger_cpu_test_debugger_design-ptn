# memfs.py
from dataclasses import dataclass, field
from typing import Dict

@dataclass
class Inode:
    is_dir: bool
    nlink: int = 1
    data: bytes = b""
    children: Dict[str, int] = field(default_factory=dict)

class MemFS:
    def __init__(self):
        self.inodes: Dict[int, Inode] = {}
        self.next_ino = 1
        # ルートディレクトリ
        self.root = self._new_inode(is_dir=True)

    def _new_inode(self, is_dir: bool) -> int:
        ino = self.next_ino
        self.next_ino += 1
        self.inodes[ino] = Inode(is_dir=is_dir)
        return ino

    def ls(self):
        return list(self.inodes[self.root].children.keys())

    def create(self, name: str):
        root = self.inodes[self.root]
        if name in root.children:
            raise FileExistsError(name)
        ino = self._new_inode(is_dir=False)
        root.children[name] = ino

    def write(self, name: str, data: bytes):
        ino = self._get_inode_by_name(name)
        inode = self.inodes[ino]
        if inode.is_dir:
            raise IsADirectoryError(name)
        inode.data = data

    def read(self, name: str) -> bytes:
        ino = self._get_inode_by_name(name)
        inode = self.inodes[ino]
        if inode.is_dir:
            raise IsADirectoryError(name)
        return inode.data

    def unlink(self, name: str):
        root = self.inodes[self.root]
        ino = self._get_inode_by_name(name)
        del root.children[name]
        inode = self.inodes[ino]
        inode.nlink -= 1
        if inode.nlink == 0:
            del self.inodes[ino]

    def rename(self, old: str, new: str):
        root = self.inodes[self.root]
        ino = self._get_inode_by_name(old)
        if new in root.children:
            raise FileExistsError(new)
        del root.children[old]
        root.children[new] = ino

    def link(self, src: str, dst: str):
        root = self.inodes[self.root]
        ino = self._get_inode_by_name(src)
        if dst in root.children:
            raise FileExistsError(dst)
        root.children[dst] = ino
        self.inodes[ino].nlink += 1

    def _get_inode_by_name(self, name: str) -> int:
        root = self.inodes[self.root]
        if name not in root.children:
            raise FileNotFoundError(name)
        return root.children[name]

# 使い方例
if __name__ == "__main__":
    fs = MemFS()
    fs.create("a.txt")
    fs.write("a.txt", b"hello")
    print(fs.read("a.txt"))          # b'hello'
    fs.link("a.txt", "b.txt")
    print(fs.ls())                   # ['a.txt', 'b.txt']
    fs.unlink("a.txt")
    print(fs.read("b.txt"))          # b'hello'