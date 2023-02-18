CREATE DATABASE atmospheric;
\c atmospheric

CREATE TABLE sensor_observations(
    datetime TIMESTAMP NOT NULL,
    device VARCHAR(255) NOT NULL,
    temp_dht11 float NOT NULL,
    temp_ds18b20 float NOT NULL,
    humidity_dht11 float NOT NULL,

    PRIMARY KEY (datetime, device)
);

