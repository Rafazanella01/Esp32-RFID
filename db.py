import asyncpg
from dotenv import load_dotenv
import os

load_dotenv()
databaseUrl = os.getenv("DATABASE_URL") # Carrega as variaveis de ambiente para uso do BD

async def query(pedido, params = None): # Recebe uma query e se tiver parametros
    conn = None
    try:
        conn = await asyncpg.connect(databaseUrl) # Cria conexão

        if params: # Se tiver parametros passa eles, caso contrario apenas executa a query
            result = await conn.fetch(pedido, *params) 
        else:
            result = await conn.fetch(pedido)

        return result

    except Exception as e: # Tratamento de erro
        print((f'Erro causado: {e}'))
        return ('500')
    
    finally:
        if conn:
            await conn.close() # Fecha a conexão
