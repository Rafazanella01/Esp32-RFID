import sqlite3

class Db:
    
    def __init__(self):
        self.con = sqlite3.connect("teste.db")
        self.cursor = self.con.cursor()
        
        self.cursor.execute("""CREATE TABLE IF NOT EXISTS teste (
            Id INTEGER PRIMARY KEY AUTOINCREMENT,
            Nome TEXT,
            Email TEXT)""")
        
    def inserirDados(self, query):
        self.cursor.execute("INSERT INTO teste (Nome, Email) VALUES (?, ?)", query) 
        self.con.commit()  
        
    def listarDados(self):
        self.cursor.execute("SELECT * FROM teste")
        dados = self.cursor.fetchall()
        
        if dados:
            for registro in dados:
                id, nome, email = registro
                
                print(f"ID: {id}, Nome: {nome}, Email: {email}")
        else:
            print("Nenhim dado encontrado!")
        
    def fecharConexao(self):
        self.con.close() 