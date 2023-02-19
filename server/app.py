import toml
import pg8000.native as pg

from flask import Flask, request


app = Flask(__name__)
settings = toml.load("./settings.toml")


def __submit_sql_transaction(sql: str):
    with pg.Connection(**settings['database']) as conn:
        return conn.run(sql)


@app.route("/", methods = ["POST"])
def insert_sensor_observations():
    """
    Insert information contained in 'data' using the 'database_settings'.

    Parameters
    ----------
    data : dict
        Python dictionary with the information to be inserted in the
        'sensor_observations' table.
    """
    sql = f"""
        INSERT INTO public.sensor_observations
        VALUES (
            '{request.form["datetime"]}',
            '{request.form["device"]}', 
            {request.form["temp_dht11"]}, 
            {request.form["temp_ds18b20"]},
            {request.form["humidity_dht11"]}
        );
        """
    __submit_sql_transaction(sql)
    return {"inserted_information": request.form}


@app.route("/obs_between/<start>/<end>", methods = ["GET"])
def get_sensor_observations(start: str, end: str):
    """
    Retrieve the observations between 'start' and 'end' (inclusive).

    Parameters
    ----------
    start : str
        Period start in the format "YYYY-MM-DDTHH:mm:ss"
    end : str
        Period end in the format "YYYY-MM-DDTHH:mm:ss"
    """
    sql = f"""
        SELECT *
        FROM
            public.sensor_observations
        WHERE
            datetime BETWEEN '{start}' AND '{end}'
        """
    return __submit_sql_transaction(sql)
