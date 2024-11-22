from flask import Flask, request, Response, jsonify, render_template, redirect
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
    return render_template("index.html")

@app.route('/logs')
async def logs():
    rfid = request.args.get("id")
    status = request.args.get("status")
    logs = []

    if rfid and status:
        # Prepara a consulta SQL com parâmetros para evitar SQL injection
        queryString = "INSERT INTO logs(rfid, status) VALUES ($1, $2);"
        result = await query(queryString, (rfid, status))  # Passando os parâmetros como tupla

        # print ('response return to api: ' + result)

        # Retorna uma resposta para o cliente
        return Response(status=200)

    response = await query("SELECT rfid, status, log_date FROM logs WHERE id IS NOT NULL;")
    
    for i in response:
        logs.append({
            "rfid": i[0],
            "status": i[1],
            "data": i[2].strftime('%a, %d %b %Y %H:%M:%S GMT')  # Format datetime
        })


    return jsonify(logs), 400

@app.route('/cadastros', methods=["GET", "POST"])
async def cadastro():
    email = None
    name = None

    if request.method == "POST":
        data = request.form
        email = data.get('email')
        name = data.get('name') 

        if email and name:
            queryStr = "INSERT INTO cadastros(name, email) VALUES ($1, $2);"
            await query(queryStr, (name, email))
            return redirect("/")

    return render_template("cadastros.html")

@app.route('/register', methods=["GET", "POST"])
async def register():
    if request.method == "GET":
        registers = []
        response = await query("SELECT id, name, email FROM cadastros WHERE rfid is NULL");
        
        for i in response:
            registers.append({"name": i[1], "email": i[2]})

        if not registers:
            return Response("Sem cadastros pendentes!",status=200)
        return jsonify(registers), 201

if __name__ == '__main__':
    app.run(debug=True,host="0.0.0.0", port=5000)