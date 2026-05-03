from flask import Flask, request

app = Flask(__name__)


@app.route("/")
def home():
    return """
    <!DOCTYPE html>
    <html>
    <head>
        <title>Packet Inspector Demo</title>

        <style>
            *{
                margin:0;
                padding:0;
                box-sizing:border-box;
                font-family:Arial, sans-serif;
            }

            body{
                height:100vh;
                display:flex;
                justify-content:center;
                align-items:center;
                background:linear-gradient(135deg,#667eea,#764ba2);
            }

            .card{
                width:380px;
                background:white;
                padding:40px;
                border-radius:16px;
                box-shadow:0 15px 40px rgba(0,0,0,0.2);
            }

            h1{
                text-align:center;
                margin-bottom:30px;
                font-size:28px;
            }

            .subtitle{
                text-align:center;
                margin-bottom:25px;
                color:#666;
            }

            input{
                width:100%;
                padding:14px;
                margin-bottom:18px;
                border:1px solid #ddd;
                border-radius:10px;
                font-size:16px;
                outline:none;
            }

            input:focus{
                border-color:#667eea;
            }

            button{
                width:100%;
                padding:14px;
                border:none;
                border-radius:10px;
                font-size:16px;
                cursor:pointer;
                background:#667eea;
                color:white;
                font-weight:bold;
            }

            button:hover{
                opacity:0.9;
            }

            .footer{
                text-align:center;
                margin-top:20px;
                color:#777;
                font-size:14px;
            }
        </style>
    </head>

    <body>

        <div class="card">

            <h1>Packet Inspector</h1>

            <p class="subtitle">
                Generate HTTP traffic for your sniffer
            </p>

            <form action="/login" method="post">

                <input
                    name="username"
                    placeholder="Username">

                <input
                    type="password"
                    name="password"
                    placeholder="Password">

                <button type="submit">
                    Send Request
                </button>

            </form>

            <div class="footer">
                localhost:8080
            </div>

        </div>

    </body>
    </html>
    """


@app.route("/login", methods=["POST"])
def login():

    print("\\n===== HTTP FORM DATA =====")
    print(request.form)

    return """
    <h1 style="font-family:Arial;text-align:center;margin-top:100px;">
        Request Sent ✅
    </h1>
    """


app.run(
    host="0.0.0.0",
    port=8080
)
