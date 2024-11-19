from flask import Flask, request, Response, jsonify
import os
from db import query
from db_manager import create_tables
from dotenv import load_dotenv
import asyncio

app = Flask(__name__)

async def build():
    load_dotenv()
    os.system("docker compose -f infra/compose.yaml up -d")
    await create_tables()

asyncio.run(build())    

@app.route("/")
async def index():
    return 'hello world!'

@app.route('/logs')
async def logs():
    rfid = request.args.get("id")
    status = request.args.get("time")

    # Prepara a consulta SQL com parâmetros para evitar SQL injection
    queryString = "INSERT INTO logs(rfid, status) VALUES ($1, $2);"
    result = await query(queryString, (int(rfid), status))  # Passando os parâmetros como tupla

    print (result)

    # Retorna uma resposta para o cliente
    return Response(status=200)


if __name__ == '__main__':
    app.run(debug=True,host="0.0.0.0", port=5000)