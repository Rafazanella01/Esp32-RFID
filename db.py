import asyncpg
from dotenv import load_dotenv
import os

load_dotenv()
databaseUrl = os.getenv("DATABASE_URL")

async def query(pedido, params = None):
    conn = None
    try:
        conn = await asyncpg.connect(databaseUrl)

        if params:
            result = await conn.fetch(pedido, *params)
        else:
            result = await conn.fetch(pedido)

        return result

    except Exception as e:
        print((f'Erro causado: {e}'))
        return ('500')
    
    finally:
        if conn:
            await conn.close()
