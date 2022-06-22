/* Create empty database*/
CREATE DATABASE _DATABASENAME_;

USE _DATABASENAME_;

/* Build the structure */
CREATE TABLE users(
  id INT NOT NULL AUTO_INCREMENT,
  name TINYTEXT UNIQUE NOT NULL,
  password VARCHAR(64) NOT NULL,
  email TINYTEXT UNIQUE NOT NULL,
  PRIMARY KEY (id),
  INDEX idx_name (name),
  INDEX idx_email (email)
);

CREATE TABLE vehicles(
  id INT NOT NULL AUTO_INCREMENT,
  user_id INT NOT NULL,
  name TINYTEXT NOT NULL,
  color TINYTEXT NOT NULL,
  PRIMARY KEY (id),
  FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
);

CREATE TABLE sensors(
  id INT NOT NULL AUTO_INCREMENT,
  vehicle_id INT NOT NULL,
  name TINYTEXT NOT NULL,
  type ENUM('float', 'int', 'string'),
  PRIMARY KEY (id),
  FOREIGN KEY (vehicle_id) REFERENCES vehicles(id) ON DELETE CASCADE
);

CREATE TABLE samples(
  id INT NOT NULL AUTO_INCREMENT,
  timestamp TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3),
  vehicle_id INT NOT NULL,
  PRIMARY KEY (id),
  FOREIGN KEY (vehicle_id) REFERENCES vehicles(id) ON DELETE CASCADE
);

CREATE TABLE measurements(
  id INT NOT NULL AUTO_INCREMENT,
  sensor_id INT NOT NULL,
  sample_id INT NOT NULL,
  value TINYTEXT,
  PRIMARY KEY (id),
  FOREIGN KEY (sensor_id) REFERENCES sensors(id) ON DELETE CASCADE,
  FOREIGN KEY (sample_id) REFERENCES samples(id) ON DELETE CASCADE
);

