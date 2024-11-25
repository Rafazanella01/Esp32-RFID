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

@app.route('/logs', methods=["GET", "POST"])
async def logs():
    if request.method == "POST":
        data = request.get_json()
        uid = data.get("id")
        status = data.get("status")

        if uid and status:
            # Prepara a consulta SQL com parâmetros para evitar SQL injection
            queryString = "INSERT INTO logs(uid, status) VALUES ($1, $2);"
            result = await query(queryString, (uid, status))  # Passando os parâmetros como tupla

            # Retorna uma resposta para o cliente
            return Response(status=201)

        else:
           return Response(status=204) 

    if request.method == "GET":
        logs = []

        response = await query("SELECT uid, status, log_date FROM logs WHERE uid IS NOT NULL;")
        for i in response:
            logs.insert(0, {
                "uid": i[0],
                "status": i[1],
                "data": i[2].strftime('%a, %d %b %Y %H:%M:%S GMT')  # Format datetime
            })

        return jsonify(logs), 200

@app.route('/cadastros', methods=["GET", "POST"])
async def cadastro():
    email = None
    name = None

    if request.method == "POST":
        data = request.form
        email = data.get('email')
        name = data.get('name') 

        if email and name:
            queryStr = "INSERT INTO register(name, email) VALUES ($1, $2);"
            await query(queryStr, (name, email))
            return redirect("/")

    return render_template("cadastros.html")

@app.route('/register', methods=["GET", "POST"])
async def register():

    registers = []
    response = await query("SELECT uid, name, email FROM register WHERE uid is NULL");
    for i in response:
        registers.append({"name": i[1], "email": i[2]})

    if request.method == "GET":
        if not registers:
            return "Sem cadastros pendentes!", 204
        
        return jsonify(registers), 200
    
    if request.method == "POST":
        data = request.get_json()
        uid = data.get("uid")
        name = request[0].get("name")
        email = request[0].get("email")

        if uid and name and email:
            queryStr = "INSERT INTO cadastros(uid, name, email) VALUES ($1, $2, $3);"
            await query(queryStr, (uid, name, email))

            return "Cadastrado com sucesso!", 201
        
        return "Algum dado faltando!", 206

@app.route('/verify')
async def verify():
    uid = request.args.get("uid")
    if uid:
        # Prepara a consulta SQL com parâmetros para evitar SQL injection
        queryString = """
                SELECT EXISTS (
                    SELECT 1 
                    FROM cadastros 
                    WHERE uid = $1
                );"""
        result = await query(queryString, (uid,))  # Passando os parâmetros como tupla
        if result[0][0]:
            return Response(status=200)
        # Retorna uma resposta para o cliente
        return Response(status=404)
    
    return render_template("verify.html")

if __name__ == '__main__':
    app.run(debug=True,host="0.0.0.0", port=5000)