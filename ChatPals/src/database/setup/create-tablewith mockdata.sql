CREATE TABLE account
(
  user_id SERIAL PRIMARY KEY,
  username VARCHAR(50) UNIQUE NOT NULL,
  password VARCHAR(50) NOT NULL
);

CREATE TABLE room
(
  room_id SERIAL PRIMARY KEY,
  room_name VARCHAR(50) DEFAULT NULL,
  admin_id INT DEFAULT NULL,
  FOREIGN KEY (admin_id) REFERENCES account(user_id) ON DELETE SET NULL
);
-- Alter the table to allow NULL values for room_name, admin_id
-- ALTER TABLE room
-- ALTER COLUMN admin_id DROP NOT NULL;


CREATE TABLE friendship
(
  user1 INT NOT NULL,
  user2 INT NOT NULL,
  PRIMARY KEY (user1, user2),
  FOREIGN KEY (user1) REFERENCES account(user_id) ON DELETE CASCADE,
  FOREIGN KEY (user2) REFERENCES account(user_id) ON DELETE CASCADE
);

CREATE TABLE member_in_room
(
  user_id INT NOT NULL,
  room_id INT NOT NULL,
  PRIMARY KEY (user_id, room_id),
  FOREIGN KEY (user_id) REFERENCES account(user_id) ON DELETE CASCADE,
  FOREIGN KEY (room_id) REFERENCES room(room_id) ON DELETE CASCADE
);

CREATE TABLE message
(
  timestamp TIMESTAMP NOT NULL,
  msg VARCHAR(500) NOT NULL,
  user_id INT NOT NULL,
  room_id INT NOT NULL,
  PRIMARY KEY (timestamp, user_id, room_id),
  FOREIGN KEY (user_id, room_id) REFERENCES member_in_room(user_id, room_id) 
);

CREATE TABLE request
(
  user1 INT NOT NULL,
  user2 INT NOT NULL,
  PRIMARY KEY (user1, user2),
  FOREIGN KEY (user1) REFERENCES account(user_id) ON DELETE CASCADE,
  FOREIGN KEY (user2) REFERENCES account(user_id) ON DELETE CASCADE
);

-- Insert mock data into the account table
INSERT INTO account (username, password) VALUES
  ('user1', 'password1'),
  ('user2', 'password2'),
  ('user3', 'password3'),
  ('user4', 'password4'),
  ('user5', 'password5'),
  ('user6', 'password6'),
  ('user7', 'password7'),
  ('user8', 'password8');

-- Insert mock data into the room table
INSERT INTO room (room_name, admin_id) VALUES
  ('Room A', 1),
  ('Room B', 2);

-- Insert mock data into the friendship table
INSERT INTO friendship (user1, user2) VALUES
  (1, 2),
  (4, 5),
  (7, 1),
  (2, 3);

-- Insert mock data into the member_in_room table
INSERT INTO member_in_room (user_id, room_id) VALUES
  (1, 1),
  (2, 1),
  (2, 2),
  (3, 2);

-- Insert mock data into the message table
INSERT INTO message (timestamp, msg, user_id, room_id) VALUES
  ('2023-12-01 12:00:00', 'Hello, Room A!', 1, 1),
  ('2023-12-01 12:05:00', 'Hi, user1!', 2, 1),
  ('2023-12-01 12:10:00', 'Welcome to Room B!', 2, 2),
  ('2023-12-01 12:15:00', 'Thanks!', 3, 2);

-- current time
INSERT INTO message (timestamp, msg, user_id, room_id)
VALUES (CURRENT_TIMESTAMP, 'Now!', 1, 1);


