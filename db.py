import sqlite3

class Db:
    
    def __init__(self):
        self.con = sqlite3.connect("banco.db")
        self.cursor = self.con.cursor()
        
        self.cursor.execute("""CREATE TABLE IF NOT EXISTS usuarios (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nome TEXT,
            email TEXT UNIQUE,
            cargo TEXT,
            cartaoId TEXT UNIQUE)""")
        
        self.cursor.execute("""CREATE TABLE IF NOT EXISTS acessos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            usuarioId INTEGER,
            dataAcesso TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (usuarioId) REFERENCES usuarios(id))""")
    
    def fecharConexao(self):
        self.con.close()   
        
    def executarComando(self, comando, parametros=()):
        self.cursor.execute(comando, parametros)
        self.con.commit()