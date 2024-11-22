from db import query
import asyncio

async def create_tables():
    await query("""CREATE TABLE IF NOT EXISTS logs(
        id int NOT NULL PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
        rfid text NOT NULL,
        status text NOT NULL,
        log_date timestamp DEFAULT (NOW() AT TIME ZONE 'America/Sao_Paulo')
        );""")

    await query("""CREATE TABLE IF NOT EXISTS cadastros(
        id int NOT NULL PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
        name text NOT NULL,
        email text NOT NULL UNIQUE,
        rfid text UNIQUE,
        create_date timestamp DEFAULT (NOW() AT TIME ZONE 'America/Sao_Paulo'),
        esp_id int 
        );""")    
    

asyncio.run(create_tables())