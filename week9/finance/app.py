import os

import _sqlite3
from flask import Flask, flash, redirect, render_template, request, session, url_for
from flask_session import Session
from werkzeug.security import check_password_hash, generate_password_hash
from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Ensure responses aren't cached (helpful during development)
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = "0"
    response.headers["Pragma"] = "no-cache"
    return response

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = _sqlite3("sqlite:///finance.db")

# Make sure API key is set (if helpers.lookup requires it)
if not os.environ.get("API_KEY"):
    # You can uncomment the below line to raise an error if API_KEY is required.
    # raise RuntimeError("API_KEY not set")
    pass


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""
    user_id = session["user_id"]

    # Get user's cash
    rows = db.execute("SELECT cash FROM users WHERE id = ?", user_id)
    cash = rows[0]["cash"] if rows else 0.0

    # Get aggregated shares per symbol
    holdings = db.execute(
        "SELECT symbol, SUM(shares) as shares_total FROM transactions WHERE user_id = ? GROUP BY symbol HAVING shares_total > 0",
        user_id,
    )

    portfolio = []
    total = cash

    for holding in holdings:
        symbol = holding["symbol"]
        shares = holding["shares_total"]
        quote = lookup(symbol)
        if quote is None:
            # Skip or handle missing symbol (defensive)
            price = 0
            name = symbol
        else:
            price = quote["price"]
            name = quote.get("name", symbol)

        value = shares * price
        total += value

        portfolio.append(
            {
                "symbol": symbol,
                "name": name,
                "shares": shares,
                "price": usd(price),
                "value": usd(value),
            }
        )

    return render_template("index.html", portfolio=portfolio, cash=usd(cash), total=usd(total))


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    if request.method == "POST":
        symbol = request.form.get("symbol")
        shares_str = request.form.get("shares")

        # Validate symbol and shares
        if not symbol:
            return apology("must provide symbol", 400)
        if not shares_str:
            return apology("must provide number of shares", 400)

        try:
            shares = int(shares_str)
            if shares <= 0:
                return apology("shares must be positive integer", 400)
        except ValueError:
            return apology("shares must be integer", 400)

        stock = lookup(symbol)
        if stock is None:
            return apology("invalid symbol", 400)

        user_id = session["user_id"]
        rows = db.execute("SELECT cash FROM users WHERE id = ?", user_id)
        if not rows:
            return apology("user not found", 400)

        cash = rows[0]["cash"]
        cost = shares * stock["price"]

        if cash < cost:
            return apology("can't afford", 400)

        # Update user's cash
        db.execute("UPDATE users SET cash = cash - ? WHERE id = ?", cost, user_id)

        # Record transaction (positive shares)
        db.execute(
            "INSERT INTO transactions (user_id, symbol, shares, price) VALUES (?, ?, ?, ?)",
            user_id,
            stock["symbol"],
            shares,
            stock["price"],
        )

        return redirect(url_for("index"))

    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    user_id = session["user_id"]
    transactions = db.execute(
        "SELECT symbol, shares, price, transacted FROM transactions WHERE user_id = ? ORDER BY transacted DESC",
        user_id,
    )

    # Convert price and ensure data for template
    for tx in transactions:
        tx["price"] = usd(tx["price"])
        # shares carry sign: positive for buy, negative for sell

    return render_template("history.html", transactions=transactions)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""
    # Forget any user_id
    session.clear()

    if request.method == "POST":
        # Ensure username was submitted
        username = request.form.get("username")
        password = request.form.get("password")
        if not username:
            return apology("must provide username", 400)
        if not password:
            return apology("must provide password", 400)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = ?", username)

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], password):
            return apology("invalid username and/or password", 400)

        # Remember user_id
        session["user_id"] = rows[0]["id"]

        return redirect(url_for("index"))

    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""
    session.clear()
    return redirect(url_for("login"))


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    if request.method == "POST":
        symbol = request.form.get("symbol")

        if not symbol:
            return apology("must provide symbol", 400)

        stock = lookup(symbol)
        if stock is None:
            return apology("invalid symbol", 400)

        return render_template("quoted.html",
                               name=stock["name"],
                               price=usd(stock["price"]),
                               symbol=stock["symbol"])
    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    session.clear()

    if request.method == "POST":

        username = request.form.get("username")
        password = request.form.get("password")
        confirmation = request.form.get("confirmation")

        # Must provide username
        if not username:
            return apology("must provide username", 400)

        # Must provide password
        if not password:
            return apology("must provide password", 400)

        # Passwords must match
        if password != confirmation:
            return apology("passwords do not match", 400)

        # Check if username exists
        rows = db.execute("SELECT * FROM users WHERE username = ?", username)
        if len(rows) != 0:
            return apology("username already exists", 400)

        # Insert new user
        user_id = db.execute(
            "INSERT INTO users (username, hash) VALUES (?, ?)",
            username,
            generate_password_hash(password)
        )

        # Log user in
        session["user_id"] = user_id

        return redirect("/")

    else:
        return render_template("register.html")



@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    user_id = session["user_id"]

    # Get user's symbols for the sell form
    symbols = db.execute(
        "SELECT symbol, SUM(shares) as shares_total FROM transactions WHERE user_id = ? GROUP BY symbol HAVING shares_total > 0",
        user_id,
    )

    if request.method == "POST":
        symbol = request.form.get("symbol")
        shares_str = request.form.get("shares")

        if not symbol:
            return apology("must provide symbol", 400)
        if not shares_str:
            return apology("must provide number of shares", 400)

        try:
            shares = int(shares_str)
            if shares <= 0:
                return apology("shares must be positive integer", 400)
        except ValueError:
            return apology("shares must be integer", 400)

        # Check ownership
        row = db.execute(
            "SELECT SUM(shares) as shares_total FROM transactions WHERE user_id = ? AND symbol = ? GROUP BY symbol",
            user_id,
            symbol,
        )
        owned = row[0]["shares_total"] if row else 0

        if shares > owned:
            return apology("too many shares", 400)

        stock = lookup(symbol)
        if stock is None:
            return apology("invalid symbol", 400)

        # Record transaction (negative shares)
        db.execute(
            "INSERT INTO transactions (user_id, symbol, shares, price) VALUES (?, ?, ?, ?)",
            user_id,
            symbol,
            -shares,
            stock["price"],
        )

        # Update user's cash
        proceeds = shares * stock["price"]
        db.execute("UPDATE users SET cash = cash + ? WHERE id = ?", proceeds, user_id)

        return redirect(url_for("index"))

    else:
        # Render the sell template with the user's symbols
        return render_template("sell.html", symbols=[s["symbol"] for s in symbols])


# Optionally, add an /add route to add cash (some variations include this)
@app.route("/add", methods=["GET", "POST"])
@login_required
def add():
    """Add cash to user's account (optional helper route)"""
    if request.method == "POST":
        amount_str = request.form.get("amount")
        if not amount_str:
            return apology("must provide amount", 400)
        try:
            amount = float(amount_str)
            if amount <= 0:
                return apology("amount must be positive", 400)
        except ValueError:
            return apology("invalid amount", 400)

        db.execute("UPDATE users SET cash = cash + ? WHERE id = ?", amount, session["user_id"])
        return redirect(url_for("index"))
    else:
        return render_template("add.html")
