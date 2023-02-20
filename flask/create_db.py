import sqlite3 as sql

#connect to SQLite
conn = sql.connect('arduino.db')

#Create a Connection
cur = conn.cursor()

#Drop users table if already exsist.
cur.execute("DROP TABLE IF EXISTS access_log")

#Create users table  in db_web database
sql ='''CREATE TABLE "access_log" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT,
    "unix" INTEGER,
    "date_time" TEXT,
    "card_id" TEXT,
    "first_name" TEXT,
    "last_name" TEXT,
    "comments" TEXT
)'''
cur.execute(sql)

#commit changes
conn.commit()

#close the connection
conn.close()
