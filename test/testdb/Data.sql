DROP TABLE IF EXISTS t_user;
DROP TABLE IF EXISTS t_chatmsg;

CREATE TABLE t_user (
    f_user_id INT PRIMARY KEY,
    f_username VARCHAR(255),
    f_password VARCHAR(255),
    f_remark VARCHAR(255),
    f_update_time TIMESTAMP
);

CREATE TABLE t_chatmsg (
    f_msg_id INT PRIMARY KEY AUTO_INCREMENT,
    f_senderid INT,
    f_msgcontent TEXT,
    f_create_time TIMESTAMP
);

INSERT INTO t_user (f_user_id, f_username, f_password, f_remark, f_update_time)
VALUES (1, 'manjie', '123456', '会幸福吗？', CURRENT_TIMESTAMP),
       (2, 'jieni', '123456', '陶喆yyds', CURRENT_TIMESTAMP);

INSERT INTO t_chatmsg (f_senderid, f_msgcontent, f_create_time)
VALUES (1, '有人吗？', CURRENT_TIMESTAMP),
       (2, '有的有的', CURRENT_TIMESTAMP);