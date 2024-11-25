from db import query
import asyncio

async def create_tables():
    await query("""CREATE TABLE IF NOT EXISTS logs(
        id int NOT NULL PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
        uid text NOT NULL,
        status text NOT NULL,
        log_date timestamp DEFAULT (NOW() AT TIME ZONE 'America/Sao_Paulo')
        );""")

    await query("""CREATE TABLE IF NOT EXISTS cadastros(
        id int NOT NULL PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
        name text NOT NULL,
        email text NOT NULL UNIQUE,
        uid text NOT NULL UNIQUE,
        create_date timestamp DEFAULT (NOW() AT TIME ZONE 'America/Sao_Paulo'),
        esp_id int 
        );""")   

    await query("""CREATE TABLE IF NOT EXISTS register(
        id int NOT NULL PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
        name text NOT NULL,
        email text NOT NULL UNIQUE,
        uid text UNIQUE,
        create_date timestamp DEFAULT (NOW() AT TIME ZONE 'America/Sao_Paulo')
        );""")  
    

asyncio.run(create_tables())