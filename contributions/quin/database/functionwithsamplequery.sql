-- return 1 if true, 0 if false
-- Function for login (check for authorization)
CREATE OR REPLACE FUNCTION login_user(in_username VARCHAR(50), in_password VARCHAR(50))
RETURNS INTEGER AS $$
DECLARE
    user_exists INTEGER;
BEGIN
    SELECT COUNT(*) INTO user_exists
    FROM account
    WHERE username = in_username AND password = in_password;

    RETURN user_exists;
END;
$$ LANGUAGE plpgsql;

-- query for login
SELECT login_user('user1', 'password1') AS is_authorized;
SELECT login_user('user3', 'password1') AS is_authorized;



-- Drop the old register_user function
DROP FUNCTION IF EXISTS register_user(VARCHAR(50), VARCHAR(50));
-- Function for registering a new account (add an account to the database)
-- CREATE OR REPLACE FUNCTION register_user(in_username VARCHAR(50), in_password VARCHAR(50))
-- RETURNS INTEGER AS $$
-- DECLARE
--     success INTEGER;
-- BEGIN
--     INSERT INTO account (username, password)
--     VALUES (in_username, in_password);

--     GET DIAGNOSTICS success = ROW_COUNT;

--     RETURN success;
-- END;
-- $$ LANGUAGE plpgsql;
CREATE OR REPLACE FUNCTION register_user(in_username VARCHAR(50), in_password VARCHAR(50))
RETURNS INTEGER AS $$
DECLARE
    success INTEGER;
BEGIN
    -- Check if the username already exists
    IF EXISTS (SELECT 1 FROM account WHERE username = in_username) THEN
        -- Username already exists, set success to 0
        success := 0;
    ELSE
        -- Username doesn't exist, proceed with the insertion
        INSERT INTO account (username, password)
        VALUES (in_username, in_password);

        GET DIAGNOSTICS success = ROW_COUNT;
    END IF;

    RETURN success;
END;
$$ LANGUAGE plpgsql;

-- query for register
SELECT register_user('user4', 'password4') AS registration_status;
SELECT register_user('user5', 'password5') AS registration_status;

-- function to delete a user
-- CREATE OR REPLACE FUNCTION delete_user(in_username VARCHAR(50))
-- RETURNS INTEGER AS $$
-- DECLARE
--     user_id INTEGER;
--     success INTEGER;
-- BEGIN
--     -- Get user ID
--     user_id := (SELECT user_id FROM account WHERE username = in_username);

--     -- Delete user from friendship table
--     DELETE FROM friendship
--     WHERE user1 = user_id OR user2 = user_id;

--     -- Delete user from member_in_room table
--     DELETE FROM member_in_room
--     WHERE user_id = user_id;

--     -- Delete user from account table
--     DELETE FROM account
--     WHERE user_id = user_id;

--     GET DIAGNOSTICS success = ROW_COUNT;

--     RETURN success;
-- END;
-- $$ LANGUAGE plpgsql;

-- Delete user 'user5'
-- have defined cascading delete constraints in the tables, 
-- the associated records in related tables will be automatically deleted 
-- when the primary record is deleted
--DELETE FROM account WHERE username = 'user5';


-- Function to change password
CREATE OR REPLACE FUNCTION change_password(in_username VARCHAR(50), in_oldpass VARCHAR(50), in_newpass VARCHAR(50))
RETURNS INTEGER AS $$
DECLARE
    success INTEGER;
BEGIN
    UPDATE account
    SET password = in_newpass
    WHERE username = in_username AND password = in_oldpass;

    GET DIAGNOSTICS success = ROW_COUNT;

    RETURN success;
END;
$$ LANGUAGE plpgsql;
-- query change pass
SELECT change_password('user5', 'password5', 'new_password') AS password_change_status;


-- Function to get the friend list of a user
CREATE OR REPLACE FUNCTION get_friend_list(in_username VARCHAR(50))
RETURNS SETOF VARCHAR(50) AS $$
BEGIN
    RETURN QUERY
    SELECT username
    FROM account
    WHERE user_id IN (
        SELECT user2
        FROM friendship
        WHERE user1 = (SELECT user_id FROM account WHERE username = in_username)
    )
    OR user_id IN (
        SELECT user1
        FROM friendship
        WHERE user2 = (SELECT user_id FROM account WHERE username = in_username)
    );
END;
$$ LANGUAGE plpgsql;
-- query to get friend list
SELECT * FROM get_friend_list('user1');


-- Drop the old add_friend function
DROP FUNCTION IF EXISTS add_friend(VARCHAR(50), VARCHAR(50));
-- Function to add a friend (add a relationship)
CREATE OR REPLACE FUNCTION add_friend(in_username1 VARCHAR(50), in_username2 VARCHAR(50))
RETURNS INTEGER AS $$
DECLARE
    user1_id INTEGER;
    user2_id INTEGER;
    friendship_exists BOOLEAN;
    success INTEGER;
BEGIN
    -- Get user IDs
    user1_id := (SELECT user_id FROM account WHERE username = in_username1);
    user2_id := (SELECT user_id FROM account WHERE username = in_username2);

    -- Check if the friendship already exists
    SELECT EXISTS (
        SELECT 1
        FROM friendship
        WHERE (user1 = user1_id AND user2 = user2_id) OR (user1 = user2_id AND user2 = user1_id)
    ) INTO friendship_exists;

    -- If the friendship doesn't exist, insert it
    IF NOT friendship_exists THEN
        INSERT INTO friendship (user1, user2)
        VALUES (user1_id, user2_id);

        GET DIAGNOSTICS success = ROW_COUNT;
    ELSE
        -- Friendship already exists
        success := 0;
    END IF;

    RETURN success;
END;
$$ LANGUAGE plpgsql;
-- query to add a friend
SELECT add_friend('user3', 'user4') AS add_friend_status;
SELECT add_friend('user4', 'user3') AS add_friend_status;
SELECT add_friend('user5', 'user1') AS add_friend_status;

-- Function to delete a friend (remove a relationship)
CREATE OR REPLACE FUNCTION delete_friend(in_username1 VARCHAR(50), in_username2 VARCHAR(50))
RETURNS INTEGER AS $$
DECLARE
    user1_id INTEGER;
    user2_id INTEGER;
    success INTEGER;
BEGIN
    -- Get user IDs
    user1_id := (SELECT user_id FROM account WHERE username = in_username1);
    user2_id := (SELECT user_id FROM account WHERE username = in_username2);

    -- Delete the friendship
    DELETE FROM friendship
    WHERE (user1 = user1_id AND user2 = user2_id) OR (user1 = user2_id AND user2 = user1_id);

    GET DIAGNOSTICS success = ROW_COUNT;

    RETURN success;
END;
$$ LANGUAGE plpgsql;
-- query to delete a friend
SELECT delete_friend('user1', 'user5') AS delete_friend_status;



-- Function to get the room list (get a list of rooms a user belongs to)
CREATE OR REPLACE FUNCTION get_room_list(in_username VARCHAR(50))
RETURNS TABLE (room_id INT, room_name VARCHAR(50)) AS $$
BEGIN
    RETURN QUERY
    SELECT r.room_id, r.room_name
    FROM room r
    JOIN member_in_room mir ON r.room_id = mir.room_id
    WHERE mir.user_id = (SELECT user_id FROM account WHERE username = in_username);
END;
$$ LANGUAGE plpgsql;
-- query to get room list
SELECT * FROM get_room_list('user1');


-- Function to get the list of people in a room
CREATE OR REPLACE FUNCTION get_people_in_room(in_room_id INT)
RETURNS TABLE (username VARCHAR(50)) AS $$
BEGIN
    RETURN QUERY
    SELECT a.username
    FROM account a
    JOIN member_in_room mir ON a.user_id = mir.user_id
    WHERE mir.room_id = in_room_id;
END;
$$ LANGUAGE plpgsql;
-- query to get list of people in a room
SELECT * FROM get_people_in_room(1); -- if room_id = 1


-- Function to create a new room (add a room to the database)
CREATE OR REPLACE FUNCTION create_new_room(in_roomname VARCHAR(50))
RETURNS INT AS $$
DECLARE
    new_room_id INT;
BEGIN
    INSERT INTO room (room_name, admin_id)
    VALUES (in_roomname, 1) -- Assuming admin_id is 1 (you may adjust this based on your requirement)
    RETURNING room_id INTO new_room_id;

    RETURN new_room_id;
END;
$$ LANGUAGE plpgsql;
-- query to create a new room
SELECT create_new_room('Room C') AS new_room_id;


-- Function to add a person to a room
CREATE OR REPLACE FUNCTION add_person_to_room(in_userid INT, in_roomid INT)
RETURNS INTEGER AS $$
DECLARE
    success INTEGER;
BEGIN
    INSERT INTO member_in_room (user_id, room_id)
    VALUES (in_userid, in_roomid);

    GET DIAGNOSTICS success = ROW_COUNT;

    RETURN success;
END;
$$ LANGUAGE plpgsql;
-- query to add a user to a room
SELECT add_person_to_room(2, 3); -- user_id = 2 and room_id = 3


-- Function to remove a person from a room
CREATE OR REPLACE FUNCTION remove_person_from_room(in_userid INT, in_roomid INT)
RETURNS INTEGER AS $$
DECLARE
    success INTEGER;
BEGIN
    DELETE FROM member_in_room
    WHERE user_id = in_userid AND room_id = in_roomid;

    GET DIAGNOSTICS success = ROW_COUNT;

    RETURN success;
END;
$$ LANGUAGE plpgsql;
-- query to remove a user from a room
SELECT remove_person_from_room(2, 2); -- user_id = 2 and room_id = 2

-- Function to get the conversation of a room (get the recent last 100 messages in conversation from a timestamp)
CREATE OR REPLACE FUNCTION get_room_conversation(in_room_id INT, in_timestamp TIMESTAMP)
RETURNS TABLE (msg_timestamp TIMESTAMP) AS $$
BEGIN
    RETURN QUERY
    SELECT timestamp
    FROM message
    WHERE room_id = in_room_id AND timestamp < in_timestamp
    ORDER BY timestamp DESC
    LIMIT 100;
END;
$$ LANGUAGE plpgsql;
-- query to get room conversation
SELECT * FROM get_room_conversation(1, CURRENT_TIMESTAMP::TIMESTAMP);
SELECT * FROM get_room_conversation(2, CURRENT_TIMESTAMP::TIMESTAMP);

-- Drop the existing function
DROP FUNCTION IF EXISTS get_conversation_content(INT, TIMESTAMP);
-- Function to get conversation content
CREATE OR REPLACE FUNCTION get_conversation_content(in_room_id INT, in_timestamp TIMESTAMP)
RETURNS TABLE (userid INT, msg_text VARCHAR(500)) AS $$
BEGIN
    RETURN QUERY
    SELECT user_id, msg AS msg_text
    FROM message
    WHERE room_id = in_room_id AND timestamp = in_timestamp;
END;
$$ LANGUAGE plpgsql;
-- query to get content of a message sent at a timestamp
SELECT * FROM get_conversation_content(1, '2023-12-01 12:00:00'); 
-- Replace timestamp with the timestamp of the message you wanna send


-- Function to add a message to conversation
CREATE OR REPLACE FUNCTION add_message_to_conversation(in_room_id INT, in_userid INT, in_message VARCHAR(500))
RETURNS INTEGER AS $$
DECLARE
    success INTEGER;
BEGIN
    INSERT INTO message (timestamp, msg, user_id, room_id)
    VALUES (CURRENT_TIMESTAMP, in_message, in_userid, in_room_id);

    GET DIAGNOSTICS success = ROW_COUNT;

    RETURN success;
END;
$$ LANGUAGE plpgsql;
-- query to add a message to a conversation
SELECT add_message_to_conversation(1, 2, 'Hello, I am user2!') AS add_message_status; -- room_id = 1, user_id = 2

