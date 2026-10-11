"""shallow_copy.py - copy.copy makes a new outer object but shares the objects
inside it; copy.deepcopy copies those too."""
import copy


class Playlist:
    def __init__(self, name, songs):
        self.name = name
        self.songs = songs


original = Playlist("road trip", ["intro", "chorus"])
shallow = copy.copy(original)
deep = copy.deepcopy(original)

shallow.name = "copy"              # rebinding: affects only the copy
shallow.songs.append("bridge")     # mutating a shared list: affects `original` too

print("original:", original.name, original.songs)
print("shallow: ", shallow.name, shallow.songs)
print("deep:    ", deep.name, deep.songs)
print("shallow shares songs:", shallow.songs is original.songs)
print("deep shares songs:   ", deep.songs is original.songs)
