from db import *

db = Db()

db.inserirDados(("Rafael", "rafazanella2004@gmail.com"))

db.listarDados()

db.fecharConexao()

print("...")