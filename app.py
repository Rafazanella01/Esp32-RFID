from flask import Flask, request, Response, jsonify, render_template, redirect
import os
from db import query
from db_manager import create_tables
from dotenv import load_dotenv
import asyncio

app = Flask(__name__)

async def build():
    load_dotenv() # Carrega variaveis do .env
    os.system("docker compose -f infra/compose.yaml up -d") # Inicia o docker com o BD
    await create_tables() # Cria as tabelas

asyncio.run(build()) # Função para rodar a build

@app.route("/")
async def index():
    return render_template("index.html")

@app.route('/logs', methods=["GET", "POST"])
async def logs():
    if request.method == "POST": # Se vier POST (esp32) pega as informações de id e status
        data = request.get_json()
        uid = data.get("id")
        status = data.get("status")

        if uid and status:
            # Prepara a consulta SQL com parâmetros para evitar SQL injection
            queryString = "INSERT INTO logs(uid, status) VALUES ($1, $2);"
            await query(queryString, (uid, status))  # Passando os parâmetros como tupla

            # Retorna uma resposta para o cliente (criado logs)
            return Response(status=201) 

        else: # Algum erro nas infos
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
            
        # Caso for GET pega todos logs no BD e insere em uma lista, depois mostra no front
        return jsonify(logs), 200

@app.route('/cadastros', methods=["GET", "POST"])
async def cadastro():
    email = None
    name = None

    if request.method == "POST": # Recebe as infos do formulario html
        data = request.form
        email = data.get('email')
        name = data.get('name') 

        if email and name:  # Cria na tabela de register para posterior cadastro do UID pelo ESP32
            queryStr = "INSERT INTO register(name, email) VALUES ($1, $2);"
            await query(queryStr, (name, email))
            return redirect("/")
        
    return render_template("cadastros.html")

@app.route('/register', methods=["GET", "POST"])
async def register():

    # Pega os registros pendentes e guarda em uma lista
    registers = []
    response = await query("SELECT id, name, email FROM register WHERE uid is NULL");
    for i in response:
        registers.append({"id": i[0], "name": i[1], "email": i[2]})

    if request.method == "GET":
        if not registers: # Se for GET verifica se há informação na lista, se tiver info pelo hhtp code 200 que há
            return "Sem cadastros pendentes!", 204
        
        return jsonify(registers), 200
    
    if request.method == "POST": # ESP32 envia o uid para finalizar o cadastro
        data = request.get_json()
        uid = data.get("uid")

        # O restante das informações pega dos registros pendentes
        id = registers[0].get("id")
        name = registers[0].get("name")
        email = registers[0].get("email")

        if uid and name and email:
            queryStr = "INSERT INTO cadastros(uid, name, email) VALUES ($1, $2, $3);"
            await query(queryStr, (uid, name, email)) # Insere na tabela de cadastros

            queryStr = "DELETE FROM register WHERE id = $1;"
            await query(queryStr, (id,)) # Deleta da tabela de registros pendentes

            return "Cadastrado com sucesso!", 201 # Retorna para o cliente
        
        return "Algum dado faltando!", 206 # Informa erro

@app.route('/verify')
async def verify():
    uid = request.args.get("uid") # Recebe um UID para verificar se está cadastrado
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