from db import *

db = Db()

sql = """INSERT INTO usuarios (nome, email, cargo, cartaoId) VALUES (?, ?, ?, ?)"""

db.executarComando(sql,("rafael", "rafaaa@gmail.com", "estagiário", "12 43 ox oc"))

print("...")