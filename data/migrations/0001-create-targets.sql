CREATE TABLE IF NOT EXISTS Targets (
    ID INTEGER PRIMARY KEY AUTOINCREMENT,
    target_id integer,
    target_distance float,
    target_bearing float,
    target_bearing_unit text,
    target_speed float,
    target_course float,
    target_course_unit text,
    target_distance_unit text,
    target_name text,
    target_status text
);
