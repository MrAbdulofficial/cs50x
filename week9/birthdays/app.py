import os
import sqlite3
from flask import Flask, flash, jsonify, redirect, render_template, request, session

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True


# ---------------------------------------------
# Helper function to get a database connection
# ---------------------------------------------
def get_db_connection():
    conn = sqlite3.connect("birthdays.db")
    conn.row_factory = sqlite3.Row    # To access columns by name
    return conn


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/", methods=["GET", "POST"])
def index():
    if request.method == "POST":
        # Access form data
        name = request.form.get("name")
        month = request.form.get("month")
        day = request.form.get("day")

        # Insert into database
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute(
            "INSERT INTO birthdays (name, month, day) VALUES (?, ?, ?)",
            (name, month, day)
        )
        conn.commit()
        conn.close()

        return redirect("/")

    else:
        # Fetch all birthdays from DB
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute("SELECT * FROM birthdays")
        birthdays = cur.fetchall()
        conn.close()

        return render_template("index.html", birthdays=birthdays)


# Run the app
if __name__ == "__main__":
    app.run(debug=True)
