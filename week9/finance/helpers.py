import urllib.request
import urllib.error
import json

from flask import redirect, render_template, session
from functools import wraps


def apology(message, code=400):
    """Render message as an apology to user."""

    def escape(s):
        """
        Escape special characters.

        https://github.com/jacebrowning/memegen#special-characters
        """
        for old, new in [
            ("-", "--"),
            (" ", "-"),
            ("_", "__"),
            ("?", "~q"),
            ("%", "~p"),
            ("#", "~h"),
            ("/", "~s"),
            ('"', "''"),
        ]:
            s = s.replace(old, new)
        return s

    return render_template("apology.html", top=code, bottom=escape(message)), code


def login_required(f):
    """
    Decorate routes to require login.

    https://flask.palletsprojects.com/en/latest/patterns/viewdecorators/
    """

    @wraps(f)
    def decorated_function(*args, **kwargs):
        if session.get("user_id") is None:
            return redirect("/login")
        return f(*args, **kwargs)

    return decorated_function
def lookup(symbol):
    """Look up quote for symbol using CS50's API."""

    try:
        with urllib.request.urlopen(f"https://finance.cs50.io/quote?symbol={symbol}") as response:
            data = response.read().decode("utf-8")
    except urllib.error.URLError:
        return None

    try:
        json_data = json.loads(data)
        quote = json_data[0]   # <---- FIX: response is a LIST
        return {
            "name": quote["companyName"],
            "price": float(quote["latestPrice"]),
            "symbol": quote["symbol"]
        }
    except (KeyError, IndexError, TypeError, ValueError, json.JSONDecodeError):
        return None
    except (KeyError, IndexError, TypeError, ValueError):
        return None



def usd(value):
    """Format value as USD."""
    return f"${value:,.2f}"
