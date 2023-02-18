import toml
import pg8000.native as pg

from flask import Flask, request


app = Flask(__name__)
database_settings = toml.load("../settings.toml")['database']


def __insert_sensor_observations(data: dict):
    """
    Insert information contained in 'data' using the 'database_settings'.

    data : dict
        Python dictionary with the information to be inserted in the
        'sensor_observations' table.
    """
    sql = f"""
        INSERT INTO public.sensor_observations
        VALUES (
            '{data["datetime"]}',
            '{data["device"]}', 
            {data["temp_dht11"]}, 
            {data["temp_ds18b20"]},
            {data["humidity_dht11"]}
        );
        """
    with pg.Connection(**database_settings) as conn:
        conn.run(sql)


@app.route("/", methods = ["GET", "POST"])
def generic_handler():
    if request.method == "POST":
        print("recebendo requisicao, mandando inserir no banco")
        __insert_sensor_observations(request.form)
    return {"value": 10}


