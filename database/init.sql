CREATE DATABASE atmospheric;
\c atmospheric

CREATE TABLE sensor_observations(
    datetime TIMESTAMP NOT NULL,
    device VARCHAR(255) NOT NULL,
    temperature float NOT NULL,
    humidity float NOT NULL,

    PRIMARY KEY (datetime, device)
);

