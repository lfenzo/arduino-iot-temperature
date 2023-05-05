import toml
import pg8000.native as pg

from fastapi import FastAPI, Request
from pydantic import BaseModel


app = FastAPI()
settings = toml.load("./settings.toml")


class TempObs(BaseModel):
    datetime: str
    device: str
    temperature: float
    humidity: float


def __submit_sql_transaction(sql: str):
    with pg.Connection(**settings['database']) as conn:
        return conn.run(sql)


@app.get("/test")
async def hello(request: Request):
    print("requisição recebida com sucesso!")
    return "hello"


@app.get("/sensor_observations/", summary="Return all observations between the specified period.")
async def get_observations_in_period(start, end):
    """
    Parameters
    ----------
    `request : Request`
    """
    sql = f"""
        SELECT *
        FROM public.sensor_observations
        WHERE datetime BETWEEN '{start}' AND '{end}'
        """
    return __submit_sql_transaction(sql)


@app.post("/sensor_observations/", summary="Insert a temperature observation.")
async def insert_observation(obs: TempObs):
    sql = f"""
        INSERT INTO public.sensor_observations
        VALUES ('{obs.datetime}', '{obs.device}', {obs.temperature}, {obs.humidity});
        """
    __submit_sql_transaction(sql)
    return obs
