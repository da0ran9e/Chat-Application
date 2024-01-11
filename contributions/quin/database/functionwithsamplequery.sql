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
--SELECT login_user('user1', 'password1') AS is_authorized;
--SELECT login_user('user3', 'password1') AS is_authorized;



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
--SELECT register_user('user4', 'password4') AS registration_status;
--SELECT register_user('user5', 'password5') AS registration_status;

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
-- the associated records in related tables except for table message 
-- will be automatically deleted when the primary record is deleted
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
--SELECT change_password('user5', 'password5', 'new_password') AS password_change_status;


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
--SELECT * FROM get_friend_list('user1');

-- Function to get the friend request list of a user
CREATE OR REPLACE FUNCTION get_request_list(in_username VARCHAR(50))
RETURNS SETOF VARCHAR(50) AS $$
BEGIN
    RETURN QUERY
    SELECT username
    FROM account
    WHERE user_id IN (
        SELECT user1
        FROM request
        WHERE user2 = (SELECT user_id FROM account WHERE username = in_username)
    );
END;
$$ LANGUAGE plpgsql;
-- query to get friend request list
--SELECT * FROM get_request_list('user20');


-- DROP FUNCTION IF EXISTS friend_request(VARCHAR(50), VARCHAR(50));
-- Function to save friend request
CREATE OR REPLACE FUNCTION friend_request(in_username1 VARCHAR(50), in_username2 VARCHAR(50))
RETURNS INTEGER AS $$
DECLARE
    user1_id INTEGER;
    user2_id INTEGER;
    request_exists BOOLEAN;
	friendship_exists BOOLEAN;
    success INTEGER;
BEGIN
    -- Get user IDs
    SELECT user_id INTO user1_id FROM account WHERE username = in_username1;
    SELECT user_id INTO user2_id FROM account WHERE username = in_username2;

    -- Check if both users exist
    IF user1_id IS NOT NULL AND user2_id IS NOT NULL THEN
        -- Check if the request already exists
        SELECT EXISTS (
            SELECT 1
            FROM request
            WHERE (user1 = user1_id AND user2 = user2_id) OR (user1 = user2_id AND user2 = user1_id)
        ) INTO request_exists;
		
		-- Check if the friendship already exists
        SELECT EXISTS (
            SELECT 1
            FROM friendship
            WHERE (user1 = user1_id AND user2 = user2_id) OR (user1 = user2_id AND user2 = user1_id)
        ) INTO friendship_exists;

        -- If the request and friendship don't exist, insert it
        IF NOT request_exists AND NOT friendship_exists THEN
            INSERT INTO request (user1, user2)
            VALUES (user1_id, user2_id);

            GET DIAGNOSTICS success = ROW_COUNT;
        ELSE
            -- request/friendship already exists
            success := 0;
        END IF;
    ELSE
        -- Return 0 if either user doesn't exist
        success := 0;
    END IF;

    RETURN success;
END;
$$ LANGUAGE plpgsql;
-- SELECT friend_request('user13', 'user20') AS friend_request_status;


-- Drop the old add_friend function
--DROP FUNCTION IF EXISTS add_friend(VARCHAR(50), VARCHAR(50));
-- Function to add a friend (add a relationship)
CREATE OR REPLACE FUNCTION add_friend(in_username1 VARCHAR(50), in_username2 VARCHAR(50))
RETURNS INTEGER AS $$
DECLARE
    user1_id INTEGER;
    user2_id INTEGER;
    friendship_exists BOOLEAN;
	request_exists BOOLEAN;
    success INTEGER;
BEGIN
    -- Get user IDs
    SELECT user_id INTO user1_id FROM account WHERE username = in_username1;
    SELECT user_id INTO user2_id FROM account WHERE username = in_username2;

    -- Check if both users exist
    IF user1_id IS NOT NULL AND user2_id IS NOT NULL THEN
        -- Check if the friendship already exists
        SELECT EXISTS (
            SELECT 1
            FROM friendship
            WHERE (user1 = user1_id AND user2 = user2_id) OR (user1 = user2_id AND user2 = user1_id)
        ) INTO friendship_exists;
		
		SELECT EXISTS (
            SELECT 1
            FROM request
            WHERE (user1 = user1_id AND user2 = user2_id) OR (user1 = user2_id AND user2 = user1_id)
        ) INTO request_exists;
		
		IF request_exists THEN
			DELETE FROM request
			 WHERE (user1 = user1_id AND user2 = user2_id) OR (user1 = user2_id AND user2 = user1_id);
		END IF;

        -- If there's a request and the friendship doesn't exist, insert it
        IF request_exists AND NOT friendship_exists THEN
            INSERT INTO friendship (user1, user2)
            VALUES (user1_id, user2_id);

            GET DIAGNOSTICS success = ROW_COUNT;
        ELSE
            -- Friendship already exists or no request
            success := 0;
        END IF;
    ELSE
        -- Return 0 if either user doesn't exist
        success := 0;
    END IF;

    RETURN success;
END;
$$ LANGUAGE plpgsql;
-- query to add a friend
--SELECT add_friend('user3', 'user4') AS add_friend_status;
--SELECT add_friend('user4', 'user3') AS add_friend_status;
--SELECT add_friend('user20', 'user1') AS add_friend_status;

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
--SELECT delete_friend('user1', 'user5') AS delete_friend_status;


-- -- Function to get the room list (get a list of rooms a user belongs to)
-- CREATE OR REPLACE FUNCTION get_room_list(in_username VARCHAR(50))
-- RETURNS TABLE (room_id INT, room_name VARCHAR(50)) AS $$
-- BEGIN
--     RETURN QUERY
--     SELECT r.room_id, r.room_name
--     FROM room r
--     JOIN member_in_room mir ON r.room_id = mir.room_id
--     WHERE mir.user_id = (SELECT user_id FROM account WHERE username = in_username);
-- END;
-- $$ LANGUAGE plpgsql;
-- DROP FUNCTION IF EXISTS get_room_list(character varying);
-- Function to get the room list with special handling for private chats
CREATE OR REPLACE FUNCTION get_room_list(in_username VARCHAR(50))
RETURNS TABLE (room_id INT, room_display_name VARCHAR(50)) AS $$
BEGIN
    RETURN QUERY
    SELECT
        r.room_id,
        CASE
            WHEN COUNT(DISTINCT mir.user_id) < 3 THEN
                -- If less than 3 people in the room
                CASE
                    WHEN r.room_name IS NULL THEN
                        -- If room_name is NULL, return the other username
                        (SELECT a2.username
                         FROM member_in_room mir2
                         JOIN account a2 ON mir2.user_id = a2.user_id
                         WHERE mir2.room_id = r.room_id AND a2.username <> in_username
                         LIMIT 1)
                    ELSE
                        -- If room_name is not NULL, return the room_name
                        r.room_name
                END
            ELSE
                -- If 3 or more people in the room, return the room_name
                r.room_name
        END AS room_display_name
    FROM room r
    JOIN member_in_room mir ON r.room_id = mir.room_id
    JOIN account a ON mir.user_id = a.user_id
    WHERE a.username = in_username
    GROUP BY r.room_id, r.room_name;

    RETURN;
END;
$$ LANGUAGE plpgsql;
-- query to get room list
--SELECT * FROM get_room_list('user');


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

-- Function to create a private room between two users
CREATE OR REPLACE FUNCTION create_private_room(user1_username VARCHAR(50), user2_username VARCHAR(50))
RETURNS INT AS $$
DECLARE
    user1_id INT;
    user2_id INT;
    new_room_id INT;
BEGIN
    -- Get user IDs
    SELECT user_id INTO user1_id FROM account WHERE username = user1_username;
    SELECT user_id INTO user2_id FROM account WHERE username = user2_username;

    -- Check if both users exist
    IF user1_id IS NOT NULL AND user2_id IS NOT NULL THEN
        -- Create a new room with no admin and no room name
        INSERT INTO room (admin_id)
        VALUES (NULL)
        RETURNING room_id INTO new_room_id;

        -- Add users to the room
        INSERT INTO member_in_room (user_id, room_id)
        VALUES (user1_id, new_room_id), (user2_id, new_room_id);

        RETURN new_room_id;
    ELSE
        -- Return 0 if either user doesn't exist
        RETURN 0;
    END IF;
END;
$$ LANGUAGE plpgsql;
--SELECT create_private_room('user20', 'user5') AS new_room_id;


-- Function to create a new room with admin (add a room to the database)
--DROP FUNCTION IF EXISTS create_new_room(VARCHAR(50), VARCHAR(50));
CREATE OR REPLACE FUNCTION create_new_room(in_roomname VARCHAR(50), in_username VARCHAR(50))
RETURNS INT AS $$
DECLARE
    admin_id INT;
    new_room_id INT;
BEGIN
    -- Get user ID
    SELECT user_id INTO admin_id FROM account WHERE username = in_username;

    -- Check if the user exists
    IF admin_id IS NOT NULL THEN
        -- Insert new room
        INSERT INTO room (room_name, admin_id)
        VALUES (in_roomname, admin_id)
        RETURNING room_id INTO new_room_id;

        -- Insert the admin as a member of the room
        INSERT INTO member_in_room (room_id, user_id)
        VALUES (new_room_id, admin_id);

        RETURN new_room_id;
    ELSE
        -- Return 0 if the user doesn't exist
        RETURN 0;
    END IF;
END;
$$ LANGUAGE plpgsql;
--SELECT create_new_room('Room 20','user20') AS new_room_id;


-- Function to add a person to a room
CREATE OR REPLACE FUNCTION add_person_to_room(in_username VARCHAR(50), in_roomid INT)
RETURNS INTEGER AS $$
DECLARE
    in_userid INT;
    success INTEGER;
BEGIN
    -- Get user ID
    SELECT user_id INTO in_userid FROM account WHERE username = in_username;

    -- Check if both username and roomid exist
    IF in_userid IS NOT NULL AND EXISTS (SELECT 1 FROM room WHERE room_id = in_roomid) THEN
        INSERT INTO member_in_room (user_id, room_id)
        VALUES (in_userid, in_roomid);

        GET DIAGNOSTICS success = ROW_COUNT;

        RETURN success;
    ELSE
        -- Return 0 if either username or roomid doesn't exist
        RETURN 0;
    END IF;
END;
$$ LANGUAGE plpgsql;
--SELECT add_person_to_room('user14', 3); -- username = user14 and room_id = 3


-- Function to remove a person from a room
CREATE OR REPLACE FUNCTION remove_person_from_room(in_username VARCHAR(50), in_roomid INT)
RETURNS INTEGER AS $$
DECLARE
    in_userid INT;
    success INTEGER;
BEGIN
    -- Get user ID
    SELECT user_id INTO in_userid FROM account WHERE username = in_username;

    -- Check if both username and roomid exist
    IF in_userid IS NOT NULL AND EXISTS (SELECT 1 FROM room WHERE room_id = in_roomid) THEN
        DELETE FROM member_in_room
        WHERE user_id = in_userid AND room_id = in_roomid;

        GET DIAGNOSTICS success = ROW_COUNT;

        RETURN success;
    ELSE
        -- Return 0 if either username or roomid doesn't exist
        RETURN 0;
    END IF;
END;
$$ LANGUAGE plpgsql;
--SELECT remove_person_from_room('user14', 3); -- username = user14 and room_id = 2

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
--SELECT * FROM get_room_conversation(1, CURRENT_TIMESTAMP::TIMESTAMP);
--SELECT * FROM get_room_conversation(2, CURRENT_TIMESTAMP::TIMESTAMP);

-- Drop the existing function
-- DROP FUNCTION IF EXISTS get_conversation_content(INT, TIMESTAMP);
-- Function to get conversation content
CREATE OR REPLACE FUNCTION get_conversation_content(in_room_id INT, in_timestamp TIMESTAMP)
RETURNS TABLE (username VARCHAR(50), msg_text VARCHAR(500)) AS $$
BEGIN
    RETURN QUERY
    SELECT a.username, m.msg AS msg_text
    FROM message m
    JOIN account a ON m.user_id = a.user_id
    WHERE m.room_id = in_room_id AND m.timestamp = in_timestamp;
END;
$$ LANGUAGE plpgsql;
-- query to get content of a message sent at a timestamp
--SELECT * FROM get_conversation_content(1, '2023-12-01 12:00:00'); 
-- Replace timestamp with the timestamp of the message you wanna get


-- Function to add a message to conversation
CREATE OR REPLACE FUNCTION add_message_to_conversation(in_username VARCHAR(50), in_room_id INT, in_message VARCHAR(500))
RETURNS INTEGER AS $$
DECLARE
    in_userid INT;
    success INTEGER;
BEGIN
    -- Get user ID
    SELECT user_id INTO in_userid FROM account WHERE username = in_username;

    -- Check if the user exists and is in the room
    IF in_userid IS NOT NULL AND EXISTS (
        SELECT 1
        FROM member_in_room
        WHERE user_id = in_userid AND room_id = in_room_id
    ) THEN
        INSERT INTO message (timestamp, msg, user_id, room_id)
        VALUES (CURRENT_TIMESTAMP, in_message, in_userid, in_room_id);

        GET DIAGNOSTICS success = ROW_COUNT;

        RETURN success;
    ELSE
        -- Return 0 if either the user doesn't exist or is not in the room
        RETURN 0;
    END IF;
END;
$$ LANGUAGE plpgsql;
-- query to add a message to a conversation
--SELECT add_message_to_conversation('user1', 30, 'Hello, I am user1!') AS add_message_status; -- room_id = 3, username = user5

-- Function to check if the user is in the room before adding their message to a conversation 
-- CREATE OR REPLACE FUNCTION add_message_to_conversation(in_username VARCHAR(50), in_room_id INT, in_message VARCHAR(500))
-- RETURNS INTEGER AS $$
-- DECLARE
--     in_userid INT;
--     user_in_room BOOLEAN;
--     success INTEGER;
-- BEGIN
--     -- Get user ID
--     in_userid := (SELECT user_id FROM account WHERE username = in_username);

--     -- Check if the user is a member of the specified room
--     SELECT EXISTS (
--         SELECT 1
--         FROM member_in_room
--         WHERE user_id = in_userid AND room_id = in_room_id
--     ) INTO user_in_room;

--     -- If the user is in the room, proceed to insert the message
--     IF user_in_room THEN
--         INSERT INTO message (timestamp, msg, user_id, room_id)
--         VALUES (CURRENT_TIMESTAMP, in_message, in_userid, in_room_id);

--         GET DIAGNOSTICS success = ROW_COUNT;
--     ELSE
--         -- If the user is not in the room, set success to 0
--         success := 0;
--     END IF;

--     RETURN success;
-- END;
-- $$ LANGUAGE plpgsql;


--DROP FUNCTION IF EXISTS add_person_to_room(INT, INT);
--DROP FUNCTION IF EXISTS remove_person_from_room(INT, INT);
-- DROP FUNCTION IF EXISTS add_message_to_conversation(INT, INT, VARCHAR(500));


